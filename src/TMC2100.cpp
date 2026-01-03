/*
 * TMC2100 - SilentStepStick Stepper Motor Driver
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "TMC2100.h"

/*
 * Basic connection: only DIR, STEP are connected.
 * Microstepping controls should be hardwired.
 */
TMC2100::TMC2100(short steps, short dir_pin, short step_pin)
:BasicStepperDriver(steps, dir_pin, step_pin)
{}

TMC2100::TMC2100(short steps, short dir_pin, short step_pin, short enable_pin)
:BasicStepperDriver(steps, dir_pin, step_pin, enable_pin)
{}

/*
 * Fully wired.
 * All the necessary control pins for TMC2100 are connected.
 */
TMC2100::TMC2100(short steps, short dir_pin, short step_pin, short cf1_pin, short cf2_pin)
:BasicStepperDriver(steps, dir_pin, step_pin),
    cf1_pin(cf1_pin), cf2_pin(cf2_pin)
{}

TMC2100::TMC2100(short steps, short dir_pin, short step_pin, short enable_pin, short cf1_pin, short cf2_pin)
:BasicStepperDriver(steps, dir_pin, step_pin, enable_pin),
cf1_pin(cf1_pin), cf2_pin(cf2_pin)
{}

void TMC2100::begin(float rpm, short microsteps){
    BasicStepperDriver::begin(rpm, microsteps);

    if (!IS_CONNECTED(cf1_pin) || !IS_CONNECTED(cf2_pin)){
        return;
    }

    pinMode(cf1_pin, OUTPUT);
    pinMode(cf2_pin, OUTPUT);
}

/*
 * Set microstepping mode (1:divisor)
 * Allowed ranges for TMC2100 are 1:1 to 1:16
 * If the control pins are not connected, we recalculate the timing only
 */
short TMC2100::setMicrostep(short microsteps){
    BasicStepperDriver::setMicrostep(microsteps);

    if (!IS_CONNECTED(cf1_pin) || !IS_CONNECTED(cf2_pin)){
        return this->microsteps;
    }

    // CFG1 CFG2 Microsteps Mode
    // GND  GND  1          SpreadCycle
    // VIO  GND  2          SpreadCycle
    // OPEN GND  2          SpreadCycle
    // GND  VIO  4          SpreadCycle
    // VIO  VIO  16         SpreadCycle
    // OPEN VIO  4          SpreadCycle
    // GND  OPEN 8          SpreadCycle
    // VIO  OPEN 16         StealthChop
    // OPEN OPEN 16         StealthChop

    switch(this->microsteps){
        case 1:
            // GND, GND
            pinMode(cf1_pin, OUTPUT); digitalWrite(cf1_pin, LOW);
            pinMode(cf2_pin, OUTPUT); digitalWrite(cf2_pin, LOW);
            break;
        case 2:
            // VIO, GND
            pinMode(cf1_pin, OUTPUT); digitalWrite(cf1_pin, HIGH);
            pinMode(cf2_pin, OUTPUT); digitalWrite(cf2_pin, LOW);
            break;
        case 4:
            // GND, VIO
            pinMode(cf1_pin, OUTPUT); digitalWrite(cf1_pin, LOW);
            pinMode(cf2_pin, OUTPUT); digitalWrite(cf2_pin, HIGH);
            break;
        case 8:
            // GND, OPEN
            pinMode(cf1_pin, OUTPUT); digitalWrite(cf1_pin, LOW);
            pinMode(cf2_pin, INPUT);
            break;
        case 16:
            // VIO, OPEN (StealthChop)
            pinMode(cf1_pin, OUTPUT); digitalWrite(cf1_pin, HIGH);
            pinMode(cf2_pin, INPUT);
            break;
    }

    return this->microsteps;
}

short TMC2100::getMaxMicrostep(){
    return TMC2100::MAX_MICROSTEP;
}
