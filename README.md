[![arduino-library-badge](https://www.ardu-badge.com/badge/StepperDriver.svg?)](https://www.ardu-badge.com/StepperDriver)
[![Actions Status](https://github.com/laurb9/StepperDriver/workflows/PlatformIO/badge.svg)](https://github.com/laurb9/StepperDriver/actions)
[![Actions Status](https://github.com/laurb9/StepperDriver/workflows/Arduino/badge.svg)](https://github.com/laurb9/StepperDriver/actions)

StepperDriver
=============

A4988, DRV8825, DRV8834, DRV8880 and generic two-pin stepper motor driver library.
Features:
   - Constant speed mode (low rpms)
   - Linear (accelerated) speed mode, with separate acceleration and deceleration settings.
   - Non-blocking mode (yields back to caller after each pulse)
   - Early brake / increase runtime in non-blocking mode

Hardware currently supported: 
   - <a href="https://www.pololu.com/product/2134">DRV8834</a> Low-Voltage Stepper Motor Driver
     up to 1:32
   - <a href="https://www.pololu.com/product/1182">A4988</a> Stepper Motor Driver up to 1:16
   - <a href="https://www.pololu.com/product/2131">DRV8825</a> up to 1:32
   - <a href="https://www.pololu.com/product/2971">DRV8880</a> up to 1:16, with current/torque control
   - any other 2-pin stepper via DIR and STEP pins, microstepping up to 1:128 externally set

Microstepping
=============

The library can set microstepping and generate the signals for each of the support driver boards.

High RPM plus high microstep combinations may not work correctly on slower MCUs, there is a maximum speed
achieveable for each board, especially with acceleration on multiple motors at the same time.

Motors
======

- 4-wire bipolar stepper motor or 
- some 6-wire unipolar in 4-wire configuration (leaving centers out) or
- 28BYJ-48 (commonly available) with a small modification (search for "convert 28byj-48 to 4-wire").

Connections
===========

Minimal configuration from <a href="https://www.pololu.com/product/2134">Pololu DRV8834 page</a>:

<img src="https://a.pololu-files.com/picture/0J4344.600.png">

Wiring
======

This is suggested wiring for running the examples unmodified. All the pins below can be changed.

- Arduino to driver board:
    - DIR - D8
    - STEP - D9
    - GND - Arduino GND
    - GND - Motor power GND
    - VMOT - Motor power (check driver-specific voltage range)
    - A4988/DRV8825 microstep control
      - MS1/MODE0 - D10
      - MS2/MODE1 - D11
      - MS3/MODE2 - D12
    - DRV8834/DRV8880 microstep control
      - M0 - D10
      - M1 - D11
    - ~SLEEP (optional) D13

- driver board to motor (this varies from motor to motor, check motor coils schematic).
- 100uF capacitor between GND - VMOT 
- Make sure to set the max current on the driver board to the motor limit (see below).
- Have a motor power supply that can deliver that current.
- Make sure the motor power supply voltage is within the range supported by the driver board.

Set Max Current
===============

The max current is set via the potentiometer on board.
Turn it while measuring voltage at the passthrough next to it.
The formula is V = I*5*R where I=max current, R=current sense resistor installed onboard

- DRV8834 or DRV8825 Pololu boards, R=0.1 and V = 0.5 * max current(A). 
  For example, for 1A you will set it to 0.5V.

For latest info, see the Pololu board information pages.

Code
====

See the BasicStepperDriver example for a generic driver that should work with any board
supporting the DIR/STEP indexing mode.

The Microstepping example works with a DRV8834 board.

For example, to show what is possible, here is the ClockStepper example that moves a 
stepper motor like the seconds hand of a watch:

```C++
#include <Arduino.h>
#include "A4988.h"

// using a 200-step motor (most common)
#define MOTOR_STEPS 200
// configure the pins connected
#define DIR 8
#define STEP 9
#define MS1 10
#define MS2 11
#define MS3 12
A4988 stepper(MOTOR_STEPS, DIR, STEP, MS1, MS2, MS3);

void setup() {
    // Set target motor RPM to 1RPM and microstepping to 1 (full step mode)
    stepper.begin(1, 1);
}

void loop() {
    // Tell motor to rotate 360 degrees. That's it.
    stepper.rotate(360);
}
```

Hardware
========
- Arduino-compatible board
- A <a href="https://www.pololu.com/category/120/stepper-motor-drivers">stepper motor driver</a>, for example DRV8834, DRV8825, DRV8824, A4988.
- A <a href="http://www.circuitspecialists.com/stepper-motor">Stepper Motor</a>.
- 1 x 100uF capacitor
