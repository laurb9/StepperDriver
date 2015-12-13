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
 * Connection using the defaults
 */
BasicStepperDriver::BasicStepperDriver(void)
{
    init();
}

/*
 * Basic connection: only DIR, STEP are connected.
 * Microstepping controls should be hardwired.
 */
BasicStepperDriver::BasicStepperDriver(int dir_pin, int step_pin)
:dir_pin(dir_pin), step_pin(step_pin)
{
    init();
}

void BasicStepperDriver::init(void){
    pinMode(dir_pin, OUTPUT);
    digitalWrite(dir_pin, LOW);

    pinMode(step_pin, OUTPUT);
    digitalWrite(step_pin, LOW);

    setMicrostep(1);
    setRPM(RPM_DEFAULT);
}

/*
 * Set target motor RPM (1-200 is a reasonable range)
 */
void BasicStepperDriver::setRPM(unsigned rpm){
    pulse_duration_us = pulse_us(rpm, STEPS, max_microstep);
}

/*
 * DIR: forward LOW, reverse HIGH
 */
void BasicStepperDriver::setDirection(int direction){
    digitalWrite(dir_pin, (direction>=0) ? LOW : HIGH);
}

/*
 * Set stepping mode (1:divisor)
 * Allowed ranges for BasicStepperDriver are 1:1 to 1:32
 */
unsigned BasicStepperDriver::setMicrostep(unsigned divisor){
    for (unsigned ms=1; ms <= max_microstep; ms<<=1){
        if (divisor == ms){
            microsteps = divisor;
        }
    }
    return microsteps;
}

/*
 * Move the motor a given number of steps.
 * positive to move forward, negative to reverse
 */
int BasicStepperDriver::move(int steps){
    int direction = (steps >= 0) ? 1 : -1;
    steps = steps * direction;
    setDirection(direction);
    unsigned long pulse_duration = pulse_duration_us*max_microstep/microsteps/2;
    while (steps--){
        digitalWrite(step_pin, HIGH);
        DELAY_MICROS(pulse_duration);
        digitalWrite(step_pin, LOW);
        DELAY_MICROS(pulse_duration);
    }
}

/*
 * Move the motor a given number of degrees (1-360)
 */
int BasicStepperDriver::rotate(int deg){
    int steps = (long)deg * STEPS * microsteps / 360;
    return move(steps);
}
/*
 * Move the motor with sub-degree precision.
 * Note that using this function even once will add 1K to your program size
 * due to inclusion of float support.
 */
int BasicStepperDriver::rotate(double deg){
    int steps = deg * STEPS * microsteps / 360;
    return move(steps);
}
