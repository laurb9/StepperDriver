/*
 * This is not an example sketch, it is used to visualize the motor speed.
 * 
 * Usage: upload and start Tool -> Serial Plotter
 * 
 * All driver tests are done with microstep 1. Increasing microstep halves max rpm with each level.
 * The maximum usable RPM can be determined from the output.
 * The max RPM at a different microstep can be calculated with formula "max rpm / microstep"
 * 
 * Copyright (C)2020 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>

#include "BasicStepperDriver.h"
#include "MultiDriver.h"
#include "SyncDriver.h"

#define RPM 150
#define MICROSTEP 1

#define MOTOR_STEPS 200

// Do not use with a real motor, the step timing is very delayed due to serial printing.
BasicStepperDriver s1(MOTOR_STEPS, 12, 13);
BasicStepperDriver s2(MOTOR_STEPS, 12, 13);
BasicStepperDriver s3(MOTOR_STEPS, 12, 13);

void setup() {
    Serial.begin(115200);
    delay(4000);

    s1.setSpeedProfile(BasicStepperDriver::LINEAR_SPEED, 2000, 2000);
    s2.setSpeedProfile(BasicStepperDriver::LINEAR_SPEED, 500, 500);
    s3.setSpeedProfile(BasicStepperDriver::CONSTANT_SPEED);

    s1.begin(RPM, MICROSTEP);
    s2.begin(RPM, MICROSTEP);
    s3.begin(RPM, MICROSTEP);

    s1.startMove(500);
    s2.startMove(500);
    s3.startMove(500);
}

void loop() {
    unsigned w1, w2, w3;
    
    w1 = s1.nextAction();
    w2 = s2.nextAction();
    w3 = s3.nextAction();

    if (w1 > 0 || w2 > 0 || w3 > 0){
        // uncomment to see step delays instead of speed
        /*
        Serial.print(w1); 
        Serial.print("\t"); 
        Serial.print(w2); 
        Serial.print("\t"); 
        Serial.print(w2); 
        Serial.println();
        */

        // graph current rpm
        Serial.print(s1.getCurrentRPM());
        Serial.print("\t");
        Serial.print(s2.getCurrentRPM());
        Serial.print("\t");
        Serial.print(s3.getCurrentRPM());
        Serial.println();
    } else {
        delay(100000);
    }
}
