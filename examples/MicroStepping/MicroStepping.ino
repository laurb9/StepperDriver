/*
 * Microstepping demo
 *
 * This requires that M0, M1 be connected in addition to STEP,DIR
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM 120

#define DIR 8
#define STEP 9
#define ENABLE 13 // optional (just delete ENABLE from everywhere if not used)

/*
 * Choose one of the sections below that match your board
 */

#include "DRV8834.h"
#define M0 10
#define M1 11
DRV8834 stepper(MOTOR_STEPS, DIR, STEP, ENABLE, M0, M1);

// #include "A4988.h"
// #define MS1 10
// #define MS2 11
// #define MS3 12
// A4988 stepper(MOTOR_STEPS, DIR, STEP, ENABLE, MS1, MS2, MS3);

// #include "DRV8825.h"
// #define MODE0 10
// #define MODE1 11
// #define MODE2 12
// DRV8825 stepper(MOTOR_STEPS, DIR, STEP, ENABLE, MODE0, MODE1, MODE2);

// #include "DRV8880.h"
// #define M0 10
// #define M1 11
// #define TRQ0 6
// #define TRQ1 7
// DRV8880 stepper(MOTORS_STEPS, DIR, STEP, ENABLE, M0, M1, TRQ0, TRQ1);

// #include "BasicStepperDriver.h" // generic
// BasicStepperDriver stepper(DIR, STEP);

void setup() {
    /*
     * Set target motor RPM.
     * Too high will result in a high pitched whine and the motor does not move.
     */
    stepper.begin(RPM);
    stepper.enable();
    
    // set current level (for DRV8880 only). Valid percent values are 25, 50, 75 or 100.
    // stepper.setCurrent(100);
}

void loop() {
    delay(1000);

    /*
     * Moving motor at full speed is simple:
     */
    stepper.setMicrostep(1); // make sure we are in full speed mode

    // these two are equivalent: 180 degrees is 100 steps in full speed mode
    stepper.move(100);
    stepper.rotate(180);

    // one full reverse rotation
    stepper.move(-100);
    stepper.rotate(-180);

    /*
     * Microstepping mode: 1,2,4,8,16 or 32(DRV8834 only)
     * Mode 1 is full speed.
     * Mode 32 is 32 microsteps per step.
     * The motor should rotate just as fast (set RPM),
     * but movement precision is increased.
     */
    stepper.setMicrostep(8);

    // 180 degrees now takes 100 * 8 microsteps
    stepper.move(100*8);
    stepper.rotate(180);

    // as you can see, using degrees is easier
    stepper.move(-100*8);
    stepper.rotate(-180);

    delay(5000);
}
