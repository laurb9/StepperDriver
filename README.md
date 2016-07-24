StepperDriver
=============

A4988, DRV8825 and generic two-pin stepper motor driver library.
Currently supported: 
   - <a href="https://www.pololu.com/product/2134">DRV8834</a> Low-Voltage Stepper Motor Driver
     up to 1:32
   - <a href="https://www.pololu.com/product/1182">A4988</a> Stepper Motor Driver up to 1:16
   - <a href="https://www.pololu.com/product/2131">DRV8825</a> up to 1:32
   - any 2-pin stepper via DIR and STEP pins.

Motors
======

4-wire bipolar stepper motor or some 6-wire unipolar in 4-wire configuration (leaving centers out).

Connections
===========

Minimal configuration from <a href="https://www.pololu.com/product/2134">Pololu DRV8834 page</a>:

<img src="https://a.pololu-files.com/picture/0J4344.600.png">

Wiring
======

- Arduino to driver board:
    - DIR - D8
    - STEP - D9
    - SLEEP - HIGH (Vdd)
    - GND - Arduino GND
    - GND - Motor power GND
    - VMOT - Motor power (check driver-specific voltage range)
    - A4988/DRV8825 microstep control
      - MS1/MODE0 - D10
      - MS2/MODE1 - D11
      - MS3/MODE2 - D12
    - DRV8834 microstep control
      - M0 - D10
      - M1 - D11
    - ~ENABLE (optional) recommend D13 to visually check if coils are active

- driver board to motor (this varies from motor to motor, check motor coils schematic).
  I just list the motor wires counter-clockwise
    - A1 - RED
    - A2 - GRN
    - B1 - YEL
    - B2 - BLU 

- 100uF capacitor between GND - VMOT 
- Set the max current on the driver board to the motor limit (see below).
- Have a motor power supply that can deliver that current.

Set Max Current
===============

The max current is set via the potentiometer on board.
Turn it while measuring voltage at the passthrough next to it.
The formula is V = I*5*R where I=max current, R=current sense resistor installed onboard

- DRV8834 Pololu board, R=0.1 and V = 0.5 * max current(A). 
  For example, for 1A you will set it to 0.5V.

- DRV8825 low-current board, R=0.33 and V = 1.65 * max current(A).
  For example, for 0.5A the reference voltage should be 0.82V

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
// pins used are DIR, STEP, MS1, MS2, MS3 in that order
A4988 stepper(200, 8, 9, 10, 11, 12);

void setup() {
    // Set target motor RPM to 1RPM
    stepper.setRPM(1);
    // Set full speed mode (microstepping also works for smoother hand movement
    stepper.setMicrostep(1);
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

Notes
=====
For DRV8834, if 1:4 and 1:32 microstepping modes do not work, add a 470K resistor
from M0 to GND.
