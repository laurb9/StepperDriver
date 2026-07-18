/*
 * TB6600 - Toshiba Stepper Motor Driver
 * Indexer mode only.
 *
 * Copyright (C)2026 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "TB6600.h"

/*
 * Basic connection: only DIR, STEP (PUL) are connected.
 * Microstepping is configured via the board's DIP switches.
 */
TB6600::TB6600(short steps, short dir_pin, short step_pin)
:BasicStepperDriver(steps, dir_pin, step_pin)
{ initTiming(); }

TB6600::TB6600(short steps, short dir_pin, short step_pin, short enable_pin)
:BasicStepperDriver(steps, dir_pin, step_pin, enable_pin)
{ initTiming(); }

short TB6600::getMaxMicrostep(){
    return TB6600::MAX_MICROSTEP;
}
