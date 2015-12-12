/*
 * Generic Stepper Motor Driver Driver
 * Indexer mode only.
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef STEPPER_DRIVER_BASE_H
#define STEPPER_DRIVER_BASE_H
#include <Arduino.h>
#include "BasicStepperDriver.h"

// most steppers are 200 steps per revolution (1.8 degree/step)
#define STEPS 200
#define RPM_DEFAULT 180

/*
 * helper function to calculate the 1/32 step duration in micros for a given rpm value.
 */
// 60[s/min] * 1000000[us/s] / 32[microsteps] * 2[low-high] * rpm[rpm]
#define pulse_us(rpm) (937500L/rpm/STEPS)

/*
 * Basic Stepper Driver class.
 * Microstepping level should be externally controlled or hardwired.
 */
class BasicStepperDriver {
protected:
    uint8_t DIR = 8;      // DIR pin
    uint8_t STEP = 9;     // STEP pin
    uint8_t ENBL = 7;     // ~ENBL pin
    // current microstep level, must be < MICROSTEP_RANGE
    // for 1:16 microsteps is 2
    unsigned microsteps = 1;
    // step pulse duration, depends on rpm and microstep level
    unsigned pulse_duration_us = pulse_us(RPM_DEFAULT);

    void setDirection(int direction);
    void init(void);

public:
    // microstep range (1, 16, 32 etc)
    static const unsigned MICROSTEP_RANGE = 32;
    /*
     * Connection using the defaults above
     * DIR-8, STEP-9, ENBL-7
     */
    BasicStepperDriver(void);
    /*
     * Basic connection: DIR, STEP and ~ENBL are configured explicitly.
     */
    BasicStepperDriver(uint8_t dir, uint8_t step, uint8_t enbl);
    /*
     * Set current microstep level, 1=full speed, 32=fine microsteppign
     */
    void setMicrostep(int divisor);
    /*
     * Move the motor a given number of steps.
     * positive to move forward, negative to reverse
     */
    int move(int steps);
    /*
     * Energize coils to hold the position in place.
     */
    void enable(void);
    /*
     * Turn off power to allow the motor to be moved by hand.
     */
    void disable(void);
    /*
     * Set target motor RPM (1-200 is a reasonable range)
     */
    void setRPM(unsigned rpm);
};
#endif STEPPER_DRIVER_BASE_H
