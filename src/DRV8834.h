/*
 * DRV8834 - LV Stepper Motor Driver Driver (A4988-compatible - mostly)
 * Indexer mode only.
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef DRV8834_H
#define DRV8834_H
#include <Arduino.h>
#include "BasicStepperDriver.h"

class DRV8834 : public BasicStepperDriver {
protected:
    uint8_t M0 = 10;
    uint8_t M1 = 11;
public:
    // microstep range (1, 16, 32 etc)
    static const unsigned MICROSTEP_RANGE = 32;
    /*
     * Connection using the defaults above
     * DIR-8, STEP-9, M0-10, M1-11, ENBL-12
     */
    DRV8834(void);
    /*
     * Basic connection: only DIR, STEP and ~ENBL are connected.
     * Microstepping controls should be hardwired.
     */
    DRV8834(uint8_t dir, uint8_t step, uint8_t enbl);
    /*
     * Fully wired. All the necessary control pins for DRV8834 are connected.
     */
    DRV8834(uint8_t dir, uint8_t step, uint8_t enbl, uint8_t m0, uint8_t m1);
    void setMicrostep(int direction);
};
#endif DRV8834_H
