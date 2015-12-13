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
    int m0_pin = PIN_UNCONNECTED;
    int m1_pin = PIN_UNCONNECTED;
    void init(void);
public:
    // microstep range (1, 16, 32 etc)
    static const unsigned max_microstep = 32;
    /*
     * Basic connection: only DIR, STEP are connected.
     * Microstepping controls should be hardwired.
     */
    DRV8834(int dir_pin, int step_pin);
    /*
     * Fully wired. All the necessary control pins for DRV8834 are connected.
     */
    DRV8834(int dir_pin, int step_pin, int m0_pin, int m1_pin);
    unsigned setMicrostep(unsigned divisor);
};
#endif DRV8834_H
