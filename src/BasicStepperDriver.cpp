/*
 * Generic Stepper Motor Driver Driver
 * Indexer mode only.

 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "BasicStepperDriver.h"

/*
 * Basic connection: only DIR, STEP are connected.
 * Microstepping controls should be hardwired.
 */
BasicStepperDriver::BasicStepperDriver(int steps, int dir_pin, int step_pin)
:motor_steps(steps), dir_pin(dir_pin), step_pin(step_pin)
{
    init();
}

void BasicStepperDriver::init(void){
    pinMode(dir_pin, OUTPUT);
    digitalWrite(dir_pin, HIGH);

    pinMode(step_pin, OUTPUT);
    digitalWrite(step_pin, LOW);

    setMicrostep(1);
    setRPM(60); // 60 rpm is a reasonable default
}


void BasicStepperDriver::calcStepPulse(void){
    step_pulse = STEP_PULSE(rpm, motor_steps, microsteps);
}

/*
 * Set target motor RPM (1-200 is a reasonable range)
 */
void BasicStepperDriver::setRPM(unsigned rpm){
    this->rpm = rpm;
    calcStepPulse();
}

/*
 * Set stepping mode (1:microsteps)
 * Allowed ranges for BasicStepperDriver are 1:1 to 1:128
 */
unsigned BasicStepperDriver::setMicrostep(unsigned microsteps){
    for (unsigned ms=1; ms <= max_microstep; ms<<=1){
        if (microsteps == ms){
            this->microsteps = microsteps;
        }
    }
    calcStepPulse();
    return this->microsteps;
}

/*
 * DIR: forward HIGH, reverse LOW
 */
void BasicStepperDriver::setDirection(int direction){
    digitalWrite(dir_pin, (direction<0) ? LOW : HIGH);
}

/*
 * Move the motor a given number of steps.
 * positive to move forward, negative to reverse
 */
int BasicStepperDriver::move(int steps){
    int direction = (steps >= 0) ? 1 : -1;
    steps = steps * direction;
    setDirection(direction);
    /*
     * We currently try to do a 50% duty cycle so it's easy to see.
     * Other option is step_high_min, pulse_duration-step_high_min.
     */
    unsigned long pulse_duration = step_pulse/2;
    while (steps--){
        digitalWrite(step_pin, HIGH);
        unsigned long next_edge = micros() + pulse_duration;
        microWaitUntil(next_edge);
        digitalWrite(step_pin, LOW);
        microWaitUntil(next_edge + pulse_duration);
    }
}

/*
 * Move the motor a given number of degrees (1-360)
 */
int BasicStepperDriver::rotate(int deg){
    int steps = (long)deg * motor_steps * (long)microsteps / 360;
    return move(steps);
}
/*
 * Move the motor with sub-degree precision.
 * Note that using this function even once will add 1K to your program size
 * due to inclusion of float support.
 */
int BasicStepperDriver::rotate(double deg){
    int steps = deg * motor_steps * microsteps / 360;
    return move(steps);
}
