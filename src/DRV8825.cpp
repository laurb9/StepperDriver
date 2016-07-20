/*
 * DRV8825 - Stepper Motor Driver Driver
 * Indexer mode only.

 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "DRV8825.h"

/*
 * Microstepping resolution truth table (Page 13 of DRV8825 pdf)
 * 0bMODE2,MODE1,MODE0 for 1,2,4,8,16,32 microsteps
 */
const uint8_t DRV8825::MS_TABLE[] = {0b000, 0b001, 0b010, 0b011, 0b100, 0b111};

DRV8825::DRV8825(int steps, int dir_pin, int step_pin)
:A4988(steps, dir_pin, step_pin)
{}

DRV8825::DRV8825(int steps, int dir_pin, int step_pin, int enable_pin)
:A4988(steps, dir_pin, step_pin, enable_pin)
{}

/*
 * A4988-DRV8825 Compatibility map: MS1-MODE0, MS2-MODE1, MS3-MODE2
 */
DRV8825::DRV8825(int steps, int dir_pin, int step_pin, int mode0_pin, int mode1_pin, int mode2_pin)
:A4988(steps, dir_pin, step_pin, mode0_pin, mode1_pin, mode2_pin)
{}

DRV8825::DRV8825(int steps, int dir_pin, int step_pin, int enable_pin, int mode0_pin, int mode1_pin, int mode2_pin)
:A4988(steps, dir_pin, step_pin, enable_pin, mode0_pin, mode1_pin, mode2_pin)
{}

const uint8_t* DRV8825::getMicrostepTable()
{
    return (uint8_t*)DRV8825::MS_TABLE;
}

size_t DRV8825::getMicrostepTableSize()
{
    return sizeof(DRV8825::MS_TABLE);
}

unsigned DRV8825::getMaxMicrostep(){
    return DRV8825::MAX_MICROSTEP;
}
