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
const uint8_t DRV8825::msTable[] = {0b000, 0b001, 0b010, 0b011, 0b100, 0b111};

/*
 * Fully wired.
 * All the necessary control pins for DRV8825 are connected.
 * A4988-DRV8825 Compatibility map: MS1-MODE0, MS2-MODE1, MS3-MODE2
 */
DRV8825::DRV8825(uint8_t dir, uint8_t step,
                 uint8_t mode0, uint8_t mode1, uint8_t mode2)
:A4988(dir, step, mode0, mode1, mode2)
{}
