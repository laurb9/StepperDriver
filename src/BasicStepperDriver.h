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
 * helper macro
 * calculate the microstep duration in micros for a given rpm value.
 * 60[s/min] * 1000000[us/s] / microsteps / steps / rpm[rpm]
 */
#define pulse_us(rpm, steps, microsteps) ((1000000L/steps)*60/microsteps/rpm)

inline void microWaitUntil(unsigned long target_micros){
    while (micros() < target_micros);
}
#define DELAY_MICROS(us) microWaitUntil(micros() + us)

/*
 * Basic Stepper Driver class.
 * Microstepping level should be externally controlled or hardwired.
 */
class BasicStepperDriver {
protected:
    uint8_t dir_pin = 8;
    uint8_t step_pin = 9;
    // current microstep level, must be < max_microstep
    // for 1:16 microsteps is 16
    unsigned microsteps = 1;
    // step pulse duration, depends on rpm and microstep level
    unsigned long pulse_duration_us;

    void setDirection(int direction);
    void init(void);

public:
    // microstep range (1, 16, 32 etc)
    static const unsigned max_microstep = 32;
    /*
     * Connection using the defaults above
     * DIR-8, STEP-9
     */
    BasicStepperDriver(void);
    /*
     * Basic connection: DIR, STEP are configured explicitly.
     */
    BasicStepperDriver(int dir_pin, int step_pin);
    /*
     * Set current microstep level, 1=full speed, 32=fine microstepping
     * Returns new level or previous level if value out of range
     */
    unsigned setMicrostep(unsigned divisor);
    /*
     * Move the motor a given number of steps.
     * positive to move forward, negative to reverse
     */
    int move(int steps);
    /*
     * Rotate the motor a given number of degrees (1-360)
     */
    int rotate(int deg);
    /*
     * Rotate using a float or double for increased movement precision.
     */
    int rotate(double deg);
    /*
     * Set target motor RPM (1-200 is a reasonable range)
     */
    void setRPM(unsigned rpm);
};
#endif STEPPER_DRIVER_BASE_H
