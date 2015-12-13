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
    uint8_t m0_pin = 10;
    uint8_t m1_pin = 11;
public:
    // microstep range (1, 16, 32 etc)
    static const unsigned max_microstep = 32;
    /*
     * Connection using the defaults
     */
    DRV8834(void);
    /*
     * Basic connection: only DIR, STEP are connected.
     * Microstepping controls should be hardwired.
     */
    DRV8834(int dir_pin, int step_pin);
    /*
     * Fully wired. All the necessary control pins for DRV8834 are connected.
     */
    DRV8834(int dir_pin, int step_pin, int m0_pin, int m1_pin);
    void setMicrostep(int direction);
};
#endif DRV8834_H
