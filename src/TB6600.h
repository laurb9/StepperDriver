/*
 * TB6600 - Toshiba Stepper Motor Driver
 * Indexer mode only.
 *
 * TB6600 modules (PUL/DIR/ENA) set microstepping via on-board DIP switches,
 * so there are no microstep control pins. Set the same MICROSTEPS value in
 * software (begin()/setMicrostep()) as configured on the DIP switches so the
 * timing calculations are correct.
 *
 * NOTE: Common TB6600 boards use opto-isolated PUL/DIR/ENA inputs where ENA is
 * typically active LOW. If you wire ENABLE, you may need setEnableActiveState(LOW).
 *
 * Copyright (C)2026 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef TB6600_H
#define TB6600_H
#include <Arduino.h>
#include "BasicStepperDriver.h"

class TB6600 : public BasicStepperDriver {
protected:
    // Set timing requirements from TB6600 datasheet
    void initTiming(){
        // min clock (PUL) HIGH pulse duration (2.2us -> 3)
        step_high_min = 3;
        // min clock (PUL) LOW pulse duration (2.2us -> 3)
        step_low_min = 3;
        // wakeup time after ENA released (10us)
        wakeup_time = 10;
    }

    // Get max microsteps supported by the device (TB6600HG up to 1:16)
    short getMaxMicrostep() override;

private:
    // microstep range (1, 2, 4, 8, 16), configured via DIP switches
    static const short MAX_MICROSTEP = 16;

public:
    /*
     * Basic connection: only DIR, STEP (PUL) are connected.
     * Microstepping is configured via the board's DIP switches.
     */
    TB6600(short steps, short dir_pin, short step_pin);
    TB6600(short steps, short dir_pin, short step_pin, short enable_pin);
};
#endif // TB6600_H
