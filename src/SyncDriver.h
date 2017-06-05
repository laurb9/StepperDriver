/*
 * Synchronous Multi-motor group driver
 *
 * Copyright (C)2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef SYNC_DRIVER_H
#define SYNC_DRIVER_H
#include <Arduino.h>
#include "MultiDriver.h"

/*
 * Synchronous Multi-motor group driver class.
 * This driver sets up timing so all motors reach their target at the same time.
 */
class SyncDriver : public MultiDriver {
protected:
    /*
     * Generic initializer, will be called by the others
     */
    SyncDriver(const unsigned short count, Motor* const *motors)
    :MultiDriver(count, motors)
    {};

public:
    /*
     * Two-motor setup
     */
    SyncDriver(Motor& motor1, Motor& motor2)
    :SyncDriver(2, new Motor* const[2]{&motor1, &motor2})
    {};
    /*
     * Three-motor setup (X, Y, Z for example)
     */
    SyncDriver(Motor& motor1, Motor& motor2, Motor& motor3)
    :SyncDriver(3, new Motor* const[3]{&motor1, &motor2, &motor3})
    {};
    /*
     * Move the motors a given number of steps.
     * positive to move forward, negative to reverse
     */
    void move(long steps1, long steps2, long steps3=0) override;
};
#endif // SYNC_DRIVER_H
