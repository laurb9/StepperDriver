/*
 * DRV8834 - LV Stepper Motor Driver Driver (A4988-compatible - mostly)
 * Indexer mode only.

 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "drv8834.h"

/*
 * Connection using the defaults DIR-8, STEP-9, M0-10, M1-11, ENBL-12
 */
DRV8834::DRV8834(void)
{
    DRV8834::init();
}

/*
 * Basic connection: only DIR, STEP and ~ENBL are connected.
 * Microstepping controls should be hardwired.
 */
DRV8834::DRV8834(uint8_t dir, uint8_t step, uint8_t enbl)
:DIR(dir), STEP(step), ENBL(enbl)
{
    DRV8834::init();
}

/*
 * Fully wired. All the necessary control pins for DRV8834 are connected.
 */
DRV8834::DRV8834(uint8_t dir, uint8_t step, uint8_t enbl, uint8_t m0, uint8_t m1)
:DIR(dir), STEP(step), ENBL(enbl), M0(m0), M1(m1)
{
    DRV8834::init();
}

void DRV8834::init(void){
    pinMode(DIR, OUTPUT);
    digitalWrite(DIR, LOW);

    pinMode(STEP, OUTPUT);
    digitalWrite(STEP, LOW);

    pinMode(ENBL, OUTPUT);
    DRV8834::enable();

    DRV8834::stepMode(1);
    //delay(1);      // wakeup time 1ms sleep inactive to step
}

/*
 * Set target motor RPM (1-200 is a reasonable range)
 */
void DRV8834::setRPM(unsigned rpm){
    pulse_duration_us = pulse_us(rpm);
}

/*
 * Control direction pin.
 */
void DRV8834::setDirection(int direction){
    digitalWrite(DIR, (direction>=0) ? LOW : HIGH);
}

/*
 * Set stepping mode (1:divisor)
 * Allowed ranges for DRV8834 are 1:1 to 1:32
 */
void DRV8834::stepMode(int divisor){

    pinMode(M1, OUTPUT);
    digitalWrite(M1, (divisor < 8) ? LOW : HIGH);

    switch(divisor){
    case 1:
    case 8:
        pinMode(M0, OUTPUT);
        digitalWrite(M0, LOW);
        break;
    case 2:
    case 16:
        pinMode(M0, OUTPUT);
        digitalWrite(M0, HIGH);
        break;
    case 4:
    case 32:
        pinMode(M0, INPUT); // Z - high impedance
        break;
    }
    step = 32/divisor;
}

/*
 * Move the motor a given number of steps.
 * positive to move forward, negative to reverse
 */
int DRV8834::move(int steps){
    int direction = (steps >= 0) ? 1 : -1;
    steps = steps * direction;
    DRV8834::setDirection(direction);
    while (steps--){
        digitalWrite(STEP, HIGH);
        delayMicroseconds(pulse_duration_us*step);
        digitalWrite(STEP, LOW);
        delayMicroseconds(pulse_duration_us*step);
    }
}

void DRV8834::enable(void){
    // ~ENBL is active low
    digitalWrite(ENBL, LOW);
}

void DRV8834::disable(void){
    // ~ENBL is active low
    digitalWrite(ENBL, HIGH);
}
