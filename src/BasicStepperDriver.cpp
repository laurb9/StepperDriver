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
BasicStepperDriver::BasicStepperDriver(uint8_t dir, uint8_t step)
:DIR(dir), STEP(step)
{
    init();
}

void BasicStepperDriver::init(void){
    pinMode(DIR, OUTPUT);
    digitalWrite(DIR, LOW);

    pinMode(STEP, OUTPUT);
    digitalWrite(STEP, LOW);

    setMicrostep(1);
}

/*
 * Set target motor RPM (1-200 is a reasonable range)
 */
void BasicStepperDriver::setRPM(unsigned rpm){
    pulse_duration_us = pulse_us(rpm, STEPS, MICROSTEP_RANGE);
}

/*
 * DIR: forward LOW, reverse HIGH
 */
void BasicStepperDriver::setDirection(int direction){
    digitalWrite(DIR, (direction>=0) ? LOW : HIGH);
}

/*
 * Set stepping mode (1:divisor)
 * Allowed ranges for BasicStepperDriver are 1:1 to 1:32
 */
unsigned BasicStepperDriver::setMicrostep(unsigned divisor){
    for (unsigned ms=1; ms <= MICROSTEP_RANGE; ms<<=1){
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
    unsigned long pulse_duration = pulse_duration_us*MICROSTEP_RANGE/microsteps;
    while (steps--){
        digitalWrite(STEP, HIGH);
        DELAY_MICROS(pulse_duration);
        digitalWrite(STEP, LOW);
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
 * Move the motor with sub-1-degree precision.
 * Note that using this function even once will add 1K to your program size
 * due to inclusion of float support.
 */
int BasicStepperDriver::rotate(double deg){
    int steps = deg * STEPS * microsteps / 360;
    return move(steps);
}
