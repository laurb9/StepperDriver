/*
 * DRV8880 - 2A Stepper Motor Driver with AutoTune and Torque Control
 *
 * Copyright (C)2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef DRV8880_H
#define DRV8880_H
#include <Arduino.h>
#include "BasicStepperDriver.h"

class DRV8880 : public BasicStepperDriver {
protected:
    short m0 = PIN_UNCONNECTED;
    short m1 = PIN_UNCONNECTED;
    short trq0 = PIN_UNCONNECTED;
    short trq1 = PIN_UNCONNECTED;
    // tWH(STEP) pulse duration, STEP high, min value
    static const int step_high_min = 0;   // 0.47us
    // tWL(STEP) pulse duration, STEP low, min value
    static const int step_low_min = 0;    // 0.47us
    // tWAKE wakeup time, nSLEEP inactive to STEP
    static const int wakeup_time = 1500;
    // also 200ns between ENBL/DIR/Mx changes and STEP HIGH

    // Get max microsteps supported by the device
    short getMaxMicrostep() override;

private:
    // microstep range (1, 16, 32 etc)
    static const short MAX_MICROSTEP = 16;

public:
    /*
     * Basic connection: only DIR, STEP are connected.
     * Microstepping controls should be hardwired.
     */
    DRV8880(short steps, short dir_pin, short step_pin);
    DRV8880(short steps, short dir_pin, short step_pin, short enable_pin);
    /*
     * DIR, STEP and microstep control M0, M1
     */
    DRV8880(short steps, short dir_pin, short step_pin, short m0, short m1);
    DRV8880(short steps, short dir_pin, short step_pin, short enable_pin, short m0, short m1);
    /*
     * Fully Wired - DIR, STEP, microstep and current control
     */
    DRV8880(short steps, short dir_pin, short step_pin, short m0, short m1, short trq0, short trq1);
    DRV8880(short steps, short dir_pin, short step_pin, short enable_pin, short m0, short m1, short trq0, short trq1);

    void begin(float rpm=60, short microsteps=1);

    short setMicrostep(short microsteps) override;

    /*
     * Torque DAC Control
     * current percent value must be 25, 50, 75 or 100.
     */
    void setCurrent(short percent=100);
};
#endif // DRV8880_H
