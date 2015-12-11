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

static DRV8834 drv8834;

void setup() {
    /*
     * Set target motor RPM. 
     * These motors can do up to about 200rpm.
     * Too high will result in a high pitched whine and the motor does not move.
     */
    drv8834.setRPM(120);
}

void loop() {
    drv8834.enable();

    int mode = 1;
    while (mode <= 32){
        /*
         * Set microstepping mode: 1,2,4,8,16 or 32
         * Mode 1 is full speed. 
         * Mode 32 is 32 microsteps per step.
         * The motor should rotate just as fast, but movement precision is
         * increased.
         */
        drv8834.stepMode(mode);
        
        // 200*microsteps covers one full rotation for a 200-step motor
        drv8834.move(200*mode);

        mode *= 2;
    }
    // allow the motor to be moved by hand
    drv8834.disable();
    
    delay(5000);
}
