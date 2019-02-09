/*
 * Multi-motor group driver
 *
 * Copyright (C)2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef MULTI_DRIVER_H
#define MULTI_DRIVER_H
#include <Arduino.h>
#include "BasicStepperDriver.h"

#define MAX_MOTORS 3    // a reasonable but arbitrary limit
#define Motor BasicStepperDriver
/*
 * Multi-motor group driver class.
 */
class MultiDriver {
protected:
    /*
     * Configuration
     */
    unsigned short count;
    Motor* const *motors;
    /*
     * Generic initializer, will be called by the others
     */
    MultiDriver(const unsigned short count, Motor* const *motors)
    :count(count), motors(motors)
    {};

    /*
     * Movement state
     */
    // ready to start a new move
    bool ready = true;
    // when next state change is due for each motor
    unsigned long event_timers[MAX_MOTORS];
    unsigned long next_action_interval = 0;
    unsigned long last_action_end = 0;

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
    unsigned short getCount(void){
        return count;
    }
    Motor& getMotor(short index){
        return *motors[index];
    }
    /*
     * Move the motors a given number of steps.
     * positive to move forward, negative to reverse
     */
    void move(long steps1, long steps2, long steps3=0);
    void rotate(int deg1, int deg2, int deg3=0){
        rotate((long)deg1, (long)deg2, (long)deg3);
    };
    void rotate(long deg1, long deg2, long deg3=0);
    void rotate(double deg1, double deg2, double deg3=0);

    /*
     * Motor movement with external control of timing
     */
    virtual void startMove(long steps1, long steps2, long steps3=0);
    void startRotate(int deg1, int deg2, int deg3=0){
        startRotate((long)deg1, (long)deg2, (long)deg3);
    };
    void startRotate(long deg1, long deg2, long deg3=0);
    void startRotate(double deg1, double deg2, double deg3=0);
    /*
     * Toggle step and return time until next change is needed (micros)
     */
    virtual long nextAction(void);
    /*
     * Optionally, call this to begin braking to stop early
     */
    void startBrake(void);
    /*
     * State querying
     */
    bool isRunning(void);
     
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
#endif // MULTI_DRIVER_H
