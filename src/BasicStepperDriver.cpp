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
 * Basic connection: only DIR, STEP and ~ENBL are connected.
 * Microstepping controls should be hardwired.
 */
BasicStepperDriver::BasicStepperDriver(uint8_t dir, uint8_t step, uint8_t enbl)
:DIR(dir), STEP(step), ENBL(enbl)
{
    init();
}

void BasicStepperDriver::init(void){
    pinMode(DIR, OUTPUT);
    digitalWrite(DIR, LOW);

    pinMode(STEP, OUTPUT);
    digitalWrite(STEP, LOW);

    setMicrostep(1);

    enable();

    //delay(1);      // wakeup time 1ms sleep inactive to step
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
void BasicStepperDriver::setMicrostep(int divisor){
    microsteps = MICROSTEP_RANGE/divisor;
}

/*
 * Move the motor a given number of steps.
 * positive to move forward, negative to reverse
 */
int BasicStepperDriver::move(int steps){
    int direction = (steps >= 0) ? 1 : -1;
    steps = steps * direction;
    setDirection(direction);
    while (steps--){
        digitalWrite(STEP, HIGH);
        delayMicroseconds(pulse_duration_us*microsteps);
        digitalWrite(STEP, LOW);
        delayMicroseconds(pulse_duration_us*microsteps);
    }
}

void BasicStepperDriver::enable(void){
    // ~ENBL is active low
    pinMode(ENBL, OUTPUT);
    digitalWrite(ENBL, LOW);
}

void BasicStepperDriver::disable(void){
    // ~ENBL is active low
    pinMode(ENBL, OUTPUT);
    digitalWrite(ENBL, HIGH);
}
