/*
 * A4988 - Stepper Motor Driver Driver
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
    static const uint8_t ms_table[];
    int ms1_pin = PIN_UNCONNECTED;
    int ms2_pin = PIN_UNCONNECTED;
    int ms3_pin = PIN_UNCONNECTED;
    void init(void);
public:
    // microstep range (1, 16, 32 etc)
    static const unsigned max_microstep = 16;
    /*
     * Basic connection: only DIR, STEP are connected.
     * Microstepping controls should be hardwired.
     */
    A4988(int dir_pin, int step_pin);
    /*
     * Fully wired. All the necessary control pins for A4988 are connected.
     */
    A4988(int dir_pin, int step_pin, int ms1_pin, int ms2_pin, int ms3_pin);
    unsigned setMicrostep(unsigned divisor);
};
#endif A4988_H
