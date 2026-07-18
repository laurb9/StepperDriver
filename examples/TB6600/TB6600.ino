/*
 * TB6600 Stepper Driver demo
 *
 * TB6600 modules use opto-isolated PUL(STEP)/DIR/ENA inputs and set
 * microstepping via on-board DIP switches (no microstep control pins).
 * Set MICROSTEPS below to match the DIP switch selection.
 *
 * NOTE: On common TB6600 boards ENA is active LOW. If you wire ENABLE,
 * uncomment stepper.setEnableActiveState(LOW) in setup().
 *
 * Copyright (C)2026 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>
#include "TB6600.h"

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM 120

// Microstepping is set on the board's DIP switches - match it here.
// 1=full step, 2=half step etc.
#define MICROSTEPS 1

#define DIR 8
#define STEP 9
// Uncomment to use enable/disable functionality on the ENA pin
//#define ENABLE 13

// Basic 2-wire config: DIR and STEP(PUL), microstepping via DIP switches
TB6600 stepper(MOTOR_STEPS, DIR, STEP);

// Uncomment to also wire ENABLE (ENA)
//TB6600 stepper(MOTOR_STEPS, DIR, STEP, ENABLE);

void setup() {
    stepper.begin(RPM, MICROSTEPS);
    // Most TB6600 boards use active-LOW ENA - uncomment if using ENABLE:
    // stepper.setEnableActiveState(LOW);
}

void loop() {

    // energize coils - the motor will hold position
    // stepper.enable();

    /*
     * Moving motor one full revolution using the degree notation
     */
    stepper.rotate(360);

    /*
     * Moving motor to original position using steps
     */
    stepper.move(-MOTOR_STEPS*MICROSTEPS);

    // pause and allow the motor to be moved by hand
    // stepper.disable();

    delay(5000);
}
