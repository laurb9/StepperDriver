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
    static const uint8_t msTable[];
    uint8_t MODE0 = 10;
    uint8_t MODE1 = 11;
    uint8_t MODE2 = 12;

public:
    // microstep range (1, 16, 32 etc)
    static const unsigned MICROSTEP_RANGE = 32;

    DRV8825(uint8_t dir, uint8_t step,
            uint8_t mode0, uint8_t mode1, uint8_t mode2);
};
#endif DRV8825_H
