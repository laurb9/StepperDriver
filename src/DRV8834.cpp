/*
 * DRV8834 - LV Stepper Motor Driver Driver (A4988-compatible - mostly)
 * Indexer mode only.

 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "DRV8834.h"

/*
 * Connection using the defaults DIR-8, STEP-9, M0-10, M1-11, ENBL-12
 */
DRV8834::DRV8834(void)
:BasicStepperDriver()
{}

/*
 * Basic connection: only DIR, STEP and ~ENBL are connected.
 * Microstepping controls should be hardwired.
 */
DRV8834::DRV8834(uint8_t dir, uint8_t step, uint8_t enbl)
:BasicStepperDriver(dir, step, enbl)
{}

/*
 * Fully wired. All the necessary control pins for DRV8834 are connected.
 */
DRV8834::DRV8834(uint8_t dir, uint8_t step, uint8_t enbl, uint8_t m0, uint8_t m1)
:BasicStepperDriver(dir, step, enbl), M0(m0), M1(m1)
{}

/*
 * Set microstepping mode (1:divisor)
 * Allowed ranges for DRV8834 are 1:1 to 1:32
 */
void DRV8834::setMicrostep(int divisor){
    BasicStepperDriver::setMicrostep(divisor);

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
}
