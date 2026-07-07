# StepperDriver API Reference

This is the reference manual for the StepperDriver library public API. For wiring and
hardware setup see the main [README](../README.md); for working code see the sketches
under [examples/](../examples).

Class overview:

| Class | Driver board | Microstep pins | Max microstep |
|---|---|---|---|
| `BasicStepperDriver` | any STEP/DIR (indexer mode) board | none (set externally) | 1:128 |
| `A4988` | Allegro A4988 | MS1, MS2, MS3 | 1:16 |
| `DRV8825` | TI DRV8825 | MODE0, MODE1, MODE2 | 1:32 |
| `DRV8834` | TI DRV8834 (low-voltage) | M0, M1 | 1:32 |
| `DRV8880` | TI DRV8880 (with torque control) | M0, M1 (+TRQ0, TRQ1) | 1:16 |
| `TMC2100` | Trinamic TMC2100 SilentStepStick | CFG1, CFG2 | 1:16 (interpolates to 1:256 internally) |
| `MultiDriver` | 2-3 motors, independent moves | — | — |
| `SyncDriver` | 2-3 motors, synchronized moves | — | — |

All single-motor classes derive from `BasicStepperDriver` and share its API; the
driver-specific classes add microstepping pin control and correct signal timing for
their chip. `MultiDriver`/`SyncDriver` are wrappers that coordinate several
already-constructed motor objects.

## Units used throughout

- **steps** — microsteps at the *current* microstep level: one full motor revolution
  is `motor_steps * microsteps` (e.g. 200 × 16 = 3200). Pass `long`; on 8-bit AVR
  beware of `int` overflow in expressions like `100 * MOTOR_STEPS * MICROSTEPS`
  (write `100L * ...`).
- **degrees** — of output shaft rotation, independent of microstep level. `long` or
  `double` overloads; may be negative or exceed 360.
- **RPM** — output shaft revolutions per minute, `float`. 1-200 is a reasonable range.
- **acceleration / deceleration** — in full steps/s², `short` (independent of
  microstep level).

Positive steps/degrees move "forward" (DIR pin HIGH), negative reverse.

## Construction and setup

Every class takes the motor's full steps per revolution (usually 200 for 1.8°
motors, 400 for 0.9°) followed by the pins. All pins are optional beyond DIR and
STEP; each class provides constructors for the common wirings:

```C++
BasicStepperDriver(steps, dir_pin, step_pin);
BasicStepperDriver(steps, dir_pin, step_pin, enable_pin);

A4988(steps, dir_pin, step_pin);
A4988(steps, dir_pin, step_pin, enable_pin);
A4988(steps, dir_pin, step_pin, ms1_pin, ms2_pin, ms3_pin);
A4988(steps, dir_pin, step_pin, enable_pin, ms1_pin, ms2_pin, ms3_pin);

DRV8825(steps, dir_pin, step_pin [, enable_pin] [, mode0_pin, mode1_pin, mode2_pin]);
DRV8834(steps, dir_pin, step_pin [, enable_pin] [, m0_pin, m1_pin]);
DRV8880(steps, dir_pin, step_pin [, enable_pin] [, m0, m1] [, trq0, trq1]);
TMC2100(steps, dir_pin, step_pin [, enable_pin] [, cfg1_pin, cfg2_pin]);
```

If the microstep pins are not connected, set the level on the board (jumpers/DIP
switches) and pass the same value to `begin()` so the timing math is correct.

### `void begin(float rpm=60, short microsteps=1)`

Initializes the pins and stores the target speed and microstep level. Call once from
`setup()` before any movement. Also energizes the motor (calls `enable()`).

### `short setMicrostep(short microsteps)`

Sets the microstep level (must be a power of 2 within the driver's supported range).
On driver classes with connected mode pins this programs the board; on
`BasicStepperDriver` it only updates the timing math. Returns the level in effect
(unchanged if the requested value was invalid).
`short getMicrostep()` returns the current level, `short getSteps()` the motor's
full steps per revolution.

### `void setRPM(float rpm)` / `float getRPM()`

Sets/returns the target speed. Takes effect at the next move (the current move's
profile is precalculated). `float getCurrentRPM()` returns the momentary speed based
on the last step interval — during acceleration it differs from the target.

### Speed profiles: `setSpeedProfile()`

```C++
enum Mode {CONSTANT_SPEED, LINEAR_SPEED};
void setSpeedProfile(Mode mode, short accel=1000, short decel=1000);
void setSpeedProfile(struct Profile profile);   // {mode, accel, decel}
struct Profile getSpeedProfile();
short getAcceleration();  short getDeceleration();
```

- **`CONSTANT_SPEED`** (default): every step fires at the fixed target RPM interval.
  Simple and lightest on the MCU, but starting a motor at high RPM from standstill
  can stall it or lose steps — keep RPM low or use LINEAR_SPEED.
- **`LINEAR_SPEED`**: trapezoidal profile — accelerate at `accel` [full steps/s²] up
  to the target RPM, cruise, then decelerate at `decel`. If the move is too short to
  reach the target RPM, the profile becomes triangular (accelerate, then brake
  early). The ramp parameters are precalculated at move start; changing RPM or the
  profile mid-move has no effect until the next move.

High RPM combined with high microstep levels is limited by MCU speed — at some point
the step interval becomes shorter than the time needed to compute it. The UnitTest
example reports achievable rates for a given board.

## Blocking moves

```C++
void move(long steps);        // in microsteps, negative = reverse
void rotate(long deg);        // in degrees of shaft rotation
void rotate(double deg);      // sub-degree precision (pulls in float code)
```

These return only after the motion completes. Example — one full turn forward and
back, accelerated:

```C++
stepper.begin(120, 16);
stepper.setSpeedProfile(stepper.LINEAR_SPEED, 1000, 1000);
stepper.rotate(360);
stepper.move(-200L * 16);     // same rotation back, expressed in microsteps
```

`long calcStepsForRotation(long deg)` (and a `double` overload) converts degrees to
microsteps at the current settings, which is what `rotate()` uses internally.

## Non-blocking moves

Start a move, then call `nextAction()` from your loop as often as possible; it waits
out any remaining time in the current step interval, fires the step pulse, and
returns the time available until the next one:

```C++
void startMove(long steps, long time=0);   // time (µs): optionally stretch the move
                                           // to complete in exactly this duration
void startRotate(long deg);                // also int and double overloads
long nextAction();   // returns µs until the next event, 0 = move complete
void startBrake();   // begin decelerating to a stop now (LINEAR_SPEED);
                     // same as stop() in CONSTANT_SPEED
long stop();         // stop immediately, returns steps that were remaining
```

`nextAction()` performs the timing wait itself (up to one step interval), so calling
it in a tight loop yields correct motion; the return value tells you how much time
you have for other work before the next call is due. If you delay past the due time,
steps fire late and the motion slows/jitters.

```C++
void setup() {
    stepper.begin(120, 8);
    stepper.startRotate(5L * 360);           // five turns
}

void loop() {
    unsigned wait_time = stepper.nextAction();
    if (wait_time == 0) {                    // move finished
        stepper.disable();
    } else if (wait_time > 100) {
        // enough headroom: do other work here, keep it under wait_time µs
    }
}
```

### State queries

```C++
enum State {STOPPED, ACCELERATING, CRUISING, DECELERATING};
State getCurrentState();
long getStepsCompleted();    // steps done in the current move (positive)
long getStepsRemaining();    // steps left to complete the move (positive)
int getDirection();          // +1 forward, -1 reverse
long getTimeForMove(long steps);  // calculated µs duration of a move
```

Note: in `LINEAR_SPEED` mode `getTimeForMove()` sets up move parameters internally,
so call it before — not during — a move.

## Enable/disable

```C++
void enable();    // energize coils (hold torque); called by begin()
void disable();   // release coils (motor can be turned by hand, no holding torque)
void setEnableActiveState(short state);  // HIGH (default, nSLEEP) or LOW (nENABLE)
```

The enable pin convention depends on which driver pin you wired: ~SLEEP is active
HIGH (the library default), ~ENABLE is active LOW — call
`setEnableActiveState(LOW)` after `begin()` in that case.

## DRV8880 extras

```C++
void setCurrent(short percent=100);   // torque DAC: 25, 50, 75 or 100 (%)
```

Requires the TRQ0/TRQ1 pins to be wired (8-pin constructor).

## Multiple motors: MultiDriver and SyncDriver

Both coordinate 2 or 3 independently constructed motor objects and mirror the
single-motor API with one value per motor:

```C++
MultiDriver(Motor& m1, Motor& m2 [, Motor& m3]);   // Motor = BasicStepperDriver
SyncDriver(Motor& m1, Motor& m2 [, Motor& m3]);

void begin(float rpm=60, short microsteps=1);  // applies to all motors
void setRPM(float rpm);                        // all motors
void setMicrostep(unsigned microsteps);        // all motors
void enable();  void disable();

void move(long steps1, long steps2, long steps3=0);        // blocking
void rotate(long deg1, long deg2, long deg3=0);            // long/int/double
void startMove(long steps1, long steps2, long steps3=0);   // non-blocking
void startRotate(long deg1, long deg2, long deg3=0);
long nextAction();       // drives all motors; 0 = all moves complete
void startBrake();
Steps stop();            // immediate; returns {steps[3]} remaining per motor
bool isRunning();

unsigned short getCount();      // number of motors
Motor& getMotor(short index);   // access an individual motor (0-based)
```

- **`MultiDriver`**: each motor runs at its own speed profile; motors finish at
  different times depending on distance.
- **`SyncDriver`**: move timing is scaled so all motors arrive at their targets at
  the same time (linear interpolation of the slower axes).

Per-motor settings (speed profile, individual RPM) are made on the motor objects
themselves before starting a group move:

```C++
DRV8825 stepperX(200, DIR_X, STEP_X);
DRV8825 stepperY(200, DIR_Y, STEP_Y);
SyncDriver controller(stepperX, stepperY);

void setup() {
    controller.begin(120, 8);
}

void loop() {
    controller.rotate(90, 180);    // Y turns twice as fast, both end together
    delay(1000);
}
```

See the [MultiAxis example](../examples/MultiAxis/MultiAxis.ino) for a complete
sketch.
