/*
 * A4988 - LV Stepper Motor Driver Driver (A4988-compatible - mostly)
 * Indexer mode only.
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef A4988_H
#define A4988_H
#include <Arduino.h>
#include "BasicStepperDriver.h"

class A4988 : public BasicStepperDriver {
protected:
    static const uint8_t msTable[];
    uint8_t MS1 = 10;
    uint8_t MS2 = 11;
    uint8_t MS3 = 12;
public:
    // microstep range (1, 16, 32 etc)
    static const unsigned MICROSTEP_RANGE = 16;
    /*
     * Connection using the defaults above
     * DIR-8, STEP-9, MS1-10, MS2-11, MS3-12, ~ENBL-7
     */
    A4988(void);
    /*
     * Basic connection: only DIR, STEP and ~ENBL are connected.
     * Microstepping controls should be hardwired.
     */
    A4988(uint8_t dir, uint8_t step, uint8_t enbl);
    /*
     * Fully wired. All the necessary control pins for A4988 are connected.
     */
    A4988(uint8_t dir, uint8_t step, uint8_t enbl, uint8_t ms0, uint8_t ms1, uint8_t ms2);
    void setMicrostep(int direction);
};
#endif A4988_H
