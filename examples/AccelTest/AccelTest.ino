/*
 * Demo / test acceleration parameters
 *
 * Copyright (C)2015-2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>
#include "BasicStepperDriver.h"

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200

// Since microstepping is set externally, make sure this matches the selected mode
// 1=full step, 2=half step etc.
#define MICROSTEPS 16

#define DIR 5
#define STEP 9

// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP, 10);

void setup() {
    Serial.begin(115200);

    stepper.begin(120, MICROSTEPS);

    /*
     * LINEAR_SPEED profile needs the acceleration and deceleration values
     * in full steps / s^2.
     */
    stepper.setSpeedProfile(stepper.LINEAR_SPEED, 2000, 1000);

    Serial.println("START");
    stepper.startRotate(360);
}

void loop() {
    static int step = 0;
    unsigned wait_time = stepper.nextAction();
    if (wait_time){
        Serial.print("  step="); Serial.print(step++);
        Serial.print("  dt="); Serial.print(wait_time);
        Serial.print("  rpm="); Serial.print(stepper.getCurrentRPM());
        Serial.println();
    } else {
        stepper.disable();
        Serial.println("END");
        delay(3600000);
    }
}
