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
    // tWH(STEP) pulse duration, STEP high, min value (1.9us)
    static const int step_high_min = 2;
    // tWL(STEP) pulse duration, STEP low, min value (1.9us)
    static const int step_low_min = 2;
    // tWAKE wakeup time, nSLEEP inactive to STEP (1000us)
    static const int wakeup_time = 1000;
    // also 200ns between ENBL/DIR/Mx changes and STEP HIGH

    // Get max microsteps supported by the device
    unsigned getMaxMicrostep() override;

private:
    // microstep range (1, 16, 32 etc)
    static const unsigned MAX_MICROSTEP = 32;

public:
    /*
     * Basic connection: only DIR, STEP are connected.
     * Microstepping controls should be hardwired.
     */
    DRV8834(int steps, int dir_pin, int step_pin);
    DRV8834(int steps, int dir_pin, int step_pin, int enable_pin);
    /*
     * Fully wired. All the necessary control pins for DRV8834 are connected.
     */
    DRV8834(int steps, int dir_pin, int step_pin, int m0_pin, int m1_pin);
    DRV8834(int steps, int dir_pin, int step_pin, int enable_pin, int m0_pin, int m1_pin);
    unsigned setMicrostep(unsigned microsteps);
};
#endif // DRV8834_H
