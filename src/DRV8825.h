/*
 * DRV8825 - Stepper Motor Driver Driver (A4988-compatible)
 * Indexer mode only.
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef DRV8825_H
#define DRV8825_H
#include <Arduino.h>
#include "A4988.h"

class DRV8825 : public A4988 {
protected:
    static const uint8_t ms_table[];
    int mode0_pin = PIN_UNCONNECTED;
    int mode1_pin = PIN_UNCONNECTED;
    int mode2_pin = PIN_UNCONNECTED;
    // tWH(STEP) pulse duration, STEP high, min value (1.9us)
    static const int step_high_min = 2;
    // tWL(STEP) pulse duration, STEP low, min value (1.9us)
    static const int step_low_min = 2;
    // tWAKE wakeup time, nSLEEP inactive to STEP (1000us)
    static const int wakeup_time = 1700;
    // also 650ns between ENBL/DIR/MODEx changes and STEP HIGH
public:
    // microstep range (1, 16, 32 etc)
    static const unsigned max_microstep = 32;

    DRV8825(int steps, int dir_pin, int step_pin);
    DRV8825(int steps, int dir_pin, int step_pin, int mode0_pin, int mode1_pin, int mode2_pin);
};
#endif // DRV8825_H
