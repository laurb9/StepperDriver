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
 * 0bMS1,MS2,MS3 for 1,2,4,8,16 microsteps
 */
const uint8_t A4988::msTable[] = {0b000, 0b000, 0b010, 0b110, 0b111};

/*
 * Connection using the defaults in A4988.h
 */
A4988::A4988(void)
:BasicStepperDriver()
{}

/*
 * Basic connection: only DIR, STEP and ~ENBL are connected.
 * Microstepping controls should be hardwired.
 */
A4988::A4988(uint8_t dir, uint8_t step, uint8_t enbl)
:BasicStepperDriver(dir, step, enbl)
{}

/*
 * Fully wired.
 * All the necessary control pins for A4988 are connected.
 */
A4988::A4988(uint8_t dir, uint8_t step, uint8_t enbl, uint8_t ms1, uint8_t ms2, uint8_t ms3)
:BasicStepperDriver(dir, step, enbl), MS1(ms1), MS2(ms2), MS3(ms3)
{}

/*
 * Set microstepping mode (1:divisor)
 * Allowed ranges for A4988 are 1:1 to 1:16
 */
void A4988::setMicrostep(int divisor){
    BasicStepperDriver::setMicrostep(divisor);

    pinMode(MS1, OUTPUT);
    pinMode(MS2, OUTPUT);
    pinMode(MS3, OUTPUT);

    int i = 0;
    while (i < sizeof(msTable)){
        if (divisor & 1){
            uint8_t mask = msTable[i];
            digitalWrite(MS1, mask & 4);
            digitalWrite(MS2, mask & 2);
            digitalWrite(MS3, mask & 1);
            break;
        }
        i++;
        divisor >>= 1;
    }
}
