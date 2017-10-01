/*
 * Example using non-blocking mode to move until a switch is triggered.
 *
 * Copyright (C)2015-2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>

// this pin should connect to Ground when want to stop the motor
#define STOPPER_PIN 4

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM 120
// Microstepping mode. If you hardwired it to save pins, set to the same value here.
#define MICROSTEPS 16

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
    Serial.begin(115200);

    // Configure stopper pin to read HIGH unless grounded
    pinMode(STOPPER_PIN, INPUT_PULLUP);

    stepper.begin(RPM, MICROSTEPS);
    stepper.enable();

    // set current level (for DRV8880 only). Valid percent values are 25, 50, 75 or 100.
    // stepper.setCurrent(100);

    Serial.println("START");

    // set the motor to move continuously for a reasonable time to hit the stopper
    stepper.startMove(20000);     // 20000 full steps = 100 full rotations
    // stepper.startRotate(36000);   // 36000 degrees = 100 rotations
}

void loop() {
    // first, check if stopper was hit
    if (digitalRead(STOPPER_PIN) == LOW){
        Serial.println("STOPPER REACHED");
        /*
         * In constant speed mode, startBrake() is an immediate stop but in accelerated modes
         * the motor will go a little past the stopper while braking. For immediate stop
         * we can just stop calling nextAction()
         */
         stepper.startBrake();
    }

    // motor control loop - send pulse and return how long to wait until next pulse
    unsigned wait_time_micros = stepper.nextAction();

    // 0 wait time indicates the motor has stopped
    if (wait_time_micros <= 0) {
        stepper.disable();       // comment out to keep motor powered
        delay(3600000);
    }

    // (optional) execute other code if we have enough time
    if (wait_time_micros > 100){
        // other code here
    }
}
