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
#include "DRV8834.h"
#include "A4988.h"

// All the wires needed for full functionality
#define DIR 8
#define STEP 9
#define ENBL 7

// 3-wire basic config, microstepping is hardwired on the driver
// BasicStepperDriver stepper(DIR, STEP, ENBL);

// microstep control for DRV8834
#define M0 10
#define M1 11
DRV8834 stepper(DIR, STEP, ENBL, M0, M1);

// microstep control for A4988
// #define MS1 10
// #define MS2 11
// #define MS3 12
// A4988 stepper(DIR, STEP, ENBL, MS1, MS2, MS3);

void setup() {
    /*
     * Set target motor RPM.
     * These motors can do up to about 200rpm.
     * Too high will result in a high pitched whine and the motor does not move.
     */
    stepper.setRPM(120);
}

void loop() {
    delay(1000);
    // energize coils - the motor will hold position
    stepper.enable();

    /*
     * Moving motor at full speed is simple:
     */
    stepper.setMicrostep(1); // make sure we are in full speed mode

    // one full rotation for a 200-step motor
    stepper.move(200);

    // one full reverse rotation
    stepper.move(-200);

    /*
     * Microstepping mode: 1,2,4,8,16 or 32(DRV8834 only)
     * Mode 1 is full speed.
     * Mode 32 is 32 microsteps per step.
     * The motor should rotate just as fast (set RPM),
     * but movement precision is increased.
     */
    stepper.setMicrostep(8);

    // one full rotation now takes 200 * 8 microsteps
    stepper.move(200*8);

    // pause and allow the motor to be moved by hand
    stepper.disable();

    delay(5000);
}
