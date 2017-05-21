/*
 * Multi-motor group driver
 *
 * Copyright (C)2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef MULTI_DRIVER_BASE_H
#define MULTI_DRIVER_BASE_H
#include <Arduino.h>
#include "BasicStepperDriver.h"

#define MAX_MOTORS 3    // a reasonable but arbitrary limit
#define Motor BasicStepperDriver
/*
 * Multi-motor group driver class.
 */
class MultiDriver {
public:
    unsigned short count;
    Motor* const *motors;
    /*
     * Generic initializer, will be called by the others
     */
    MultiDriver(const unsigned short count, Motor* const *motors)
    :count(count), motors(motors)
    {};

public:
    /*
     * Two-motor setup
     */
    MultiDriver(Motor& motor1, Motor& motor2)
    :MultiDriver(2, new Motor* const[2]{&motor1, &motor2})
    {};
    /*
     * Three-motor setup (X, Y, Z for example)
     */
    MultiDriver(Motor& motor1, Motor& motor2, Motor& motor3)
    :MultiDriver(3, new Motor* const[3]{&motor1, &motor2, &motor3})
    {};
    /*
     * Move the motors a given number of steps.
     * positive to move forward, negative to reverse
     */
    void move(long steps1, long steps2, long steps3=0);
    /*
     * Set the same microstepping level on all motors
     */
    void setMicrostep(unsigned microsteps);
    /*
     * Turn all motors on or off
     */
    void enable(void);
    void disable(void);
};
#endif // MULTI_DRIVER_BASE_H
