/*
 * DRV8834 - LV Stepper Motor Driver Driver (A4988-compatible - mostly)
 * Indexer mode only.
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef DRV8834_H
#include <Arduino.h>

// most steppers are 200 steps per revolution (1.8 degree/step)
#define STEPS 200
#define RPM_DEFAULT 180

// 60[s/min] * 1000000[us/s] / STEPS * 32[microsteps] * 2[low-high] * rpm[rpm]
#define pulse_us(rpm) (937500L/rpm/STEPS)

class DRV8834 {
protected:
    uint8_t DIR = 8;
    uint8_t STEP = 9;
    uint8_t M0 = 10;
    uint8_t M1 = 11;
    uint8_t ENBL = 12;
    unsigned step = 32;
    unsigned pulse_duration_us = pulse_us(RPM_DEFAULT);
    void setDirection(int direction);
    void init(void);
public:
    /*
     * Connection using the defaults above
     * DIR-8, STEP-9, M0-10, M1-11, ENBL-12
     */
    DRV8834(void);
    /*
     * Basic connection: only DIR, STEP and ~ENBL are connected.
     * Microstepping controls should be hardwired.
     */
    DRV8834(uint8_t dir, uint8_t step, uint8_t enbl);
    /*
     * Fully wired. All the necessary control pins for DRV8834 are connected.
     */
    DRV8834(uint8_t dir, uint8_t step, uint8_t m0, uint8_t m1, uint8_t enbl);
    void stepMode(int divisor);
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
#endif DRV8834_H
