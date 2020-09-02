/*
 * DRV8880 - 2A Stepper Motor Driver with AutoTune and Torque Control
 *
 * Copyright (C)2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "DRV8880.h"

/*
 * Basic connection: only DIR, STEP are connected.
 * Microstepping controls should be hardwired.
 */
DRV8880::DRV8880(short steps, short dir_pin, short step_pin)
:BasicStepperDriver(steps, dir_pin, step_pin)
{}

DRV8880::DRV8880(short steps, short dir_pin, short step_pin, short enable_pin)
:BasicStepperDriver(steps, dir_pin, step_pin, enable_pin)
{}

/*
 * Fully wired. All the necessary control pins for DRV8880 are connected.
 */
DRV8880::DRV8880(short steps, short dir_pin, short step_pin, short m0, short m1)
:BasicStepperDriver(steps, dir_pin, step_pin), m0(m0), m1(m1)
{}

DRV8880::DRV8880(short steps, short dir_pin, short step_pin, short enable_pin, short m0, short m1)
:BasicStepperDriver(steps, dir_pin, step_pin, enable_pin), m0(m0), m1(m1)
{}

DRV8880::DRV8880(short steps, short dir_pin, short step_pin, short m0, short m1, short trq0, short trq1)
:BasicStepperDriver(steps, dir_pin, step_pin), m0(m0), m1(m1), trq0(trq0), trq1(trq1)
{}

DRV8880::DRV8880(short steps, short dir_pin, short step_pin, short enable_pin, short m0, short m1, short trq0, short trq1)
:BasicStepperDriver(steps, dir_pin, step_pin, enable_pin), m0(m0), m1(m1), trq0(trq0), trq1(trq1)
{}

void DRV8880::begin(float rpm, short microsteps){
    BasicStepperDriver::begin(rpm, microsteps);
    setCurrent(100);
}

short DRV8880::getMaxMicrostep(){
    return DRV8880::MAX_MICROSTEP;
}

/*
 * Set microstepping mode (1:divisor)
 * Allowed ranges for DRV8880 are 1:1 to 1:16
 * If the control pins are not connected, we recalculate the timing only
 */
short DRV8880::setMicrostep(short microsteps){
    BasicStepperDriver::setMicrostep(microsteps);

    if (!IS_CONNECTED(m0) || !IS_CONNECTED(m1)){
        return this->microsteps;
    }

    /*
     * Step mode truth table
     * M1 M0    step mode
     *  0  0     1
     *  1  0     2
     *  1  1     4
     *  0  Z     8
     *  1  Z    16
     *
     *  0  1     2 (non-circular, not implemented)
     *  Z = high impedance mode (M0 is tri-state)
     */

    pinMode(m1, OUTPUT);
    pinMode(m0, OUTPUT);
    switch(this->microsteps){
        case 1:
            digitalWrite(m1, LOW);
            digitalWrite(m0, LOW);
            break;
        case 2:
            digitalWrite(m1, HIGH);
            digitalWrite(m0, LOW);
            break;
        case 4:
            digitalWrite(m1, HIGH);
            digitalWrite(m0, HIGH);
            break;
        case 8:
            digitalWrite(m1, LOW);
            pinMode(m0, INPUT); // Z - high impedance
            break;
        case 16:
            digitalWrite(m1, HIGH);
            pinMode(m0, INPUT); // Z - high impedance
            break;
    }
    return this->microsteps;
}

void DRV8880::setCurrent(short percent){
    /*
     * Torque DAC Settings table
     * TRQ1 TRQ0 Current scalar
     *  1     1       25%
     *  1     0       50%
     *  0     1       75%
     *  0     0      100%
     */
    if (!IS_CONNECTED(trq1) || !IS_CONNECTED(trq0)){
        return;
    }
    pinMode(trq1, OUTPUT);
    pinMode(trq0, OUTPUT);
    percent = (100-percent)/25;
    digitalWrite(trq1, percent & 2);
    digitalWrite(trq0, percent & 1);
}

