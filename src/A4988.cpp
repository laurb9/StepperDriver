/*
 * A4988 - Stepper Motor Driver Driver
 * Indexer mode only.

 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "A4988.h"

/*
 * Microstepping resolution truth table (Page 6 of A4988 pdf)
 * 0bMS3,MS2,MS1 for 1,2,4,8,16 microsteps
 */
const uint8_t A4988::MS_TABLE[] = {0b000, 0b001, 0b010, 0b011, 0b111};

/*
 * Basic connection: only DIR, STEP are connected.
 * Microstepping controls should be hardwired.
 */
A4988::A4988(int steps, int dir_pin, int step_pin)
:BasicStepperDriver(steps, dir_pin, step_pin)
{}

A4988::A4988(int steps, int dir_pin, int step_pin, int enable_pin)
:BasicStepperDriver(steps, dir_pin, step_pin, enable_pin)
{}

/*
 * Fully wired.
 * All the necessary control pins for A4988 are connected.
 */
A4988::A4988(int steps, int dir_pin, int step_pin, int ms1_pin, int ms2_pin, int ms3_pin)
:BasicStepperDriver(steps, dir_pin, step_pin),
    ms1_pin(ms1_pin), ms2_pin(ms2_pin), ms3_pin(ms3_pin)
{}

A4988::A4988(int steps, int dir_pin, int step_pin, int enable_pin, int ms1_pin, int ms2_pin, int ms3_pin)
:BasicStepperDriver(steps, dir_pin, step_pin, enable_pin),
ms1_pin(ms1_pin), ms2_pin(ms2_pin), ms3_pin(ms3_pin)
{}

void A4988::init(void){
    BasicStepperDriver::init();

    if (!IS_CONNECTED(ms1_pin) || !IS_CONNECTED(ms2_pin) || !IS_CONNECTED(ms3_pin)){
        return;
    }

    pinMode(ms1_pin, OUTPUT);
    pinMode(ms2_pin, OUTPUT);
    pinMode(ms3_pin, OUTPUT);
}

/*
 * Set microstepping mode (1:divisor)
 * Allowed ranges for A4988 are 1:1 to 1:16
 * If the control pins are not connected, we recalculate the timing only
 */
unsigned A4988::setMicrostep(unsigned microsteps){
    BasicStepperDriver::setMicrostep(microsteps);

    if (!IS_CONNECTED(ms1_pin) || !IS_CONNECTED(ms2_pin) || !IS_CONNECTED(ms3_pin)){
        return this->microsteps;
    }

    const uint8_t* ms_table = this->getMicrostepTable();
    size_t ms_table_size = this->getMicrostepTableSize();

    int i = 0;
    while (i < ms_table_size){
        if (this->microsteps & (1<<i)){
            uint8_t mask = ms_table[i];
            digitalWrite(ms3_pin, mask & 4);
            digitalWrite(ms2_pin, mask & 2);
            digitalWrite(ms1_pin, mask & 1);
            break;
        }
        i++;
    }
    return this->microsteps;
}

const uint8_t* A4988::getMicrostepTable(){
    return A4988::MS_TABLE;
}

size_t A4988::getMicrostepTableSize(){
    return sizeof(A4988::MS_TABLE);
}

unsigned A4988::getMaxMicrostep(){
    return A4988::MAX_MICROSTEP;
}
