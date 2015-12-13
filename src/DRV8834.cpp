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
 * Connection using the defaults DIR-8, STEP-9, M0-10, M1-11
 */
DRV8834::DRV8834(void)
:BasicStepperDriver()
{}

/*
 * Basic connection: only DIR, STEP are connected.
 * Microstepping controls should be hardwired.
 */
DRV8834::DRV8834(int dir_pin, int step_pin)
:BasicStepperDriver(dir_pin, step_pin)
{}

/*
 * Fully wired. All the necessary control pins for DRV8834 are connected.
 */
DRV8834::DRV8834(int dir_pin, int step_pin, int m0_pin, int m1_pin)
:BasicStepperDriver(dir_pin, step_pin), m0_pin(m0_pin), m1_pin(m1_pin)
{}

/*
 * Set microstepping mode (1:divisor)
 * Allowed ranges for DRV8834 are 1:1 to 1:32
 */
void DRV8834::setMicrostep(int divisor){
    BasicStepperDriver::setMicrostep(divisor);

    pinMode(m1_pin, OUTPUT);
    digitalWrite(m1_pin, (divisor < 8) ? LOW : HIGH);

    switch(divisor){
    case 1:
    case 8:
        pinMode(m0_pin, OUTPUT);
        digitalWrite(m0_pin, LOW);
        break;
    case 2:
    case 16:
        pinMode(m0_pin, OUTPUT);
        digitalWrite(m0_pin, HIGH);
        break;
    case 4:
    case 32:
        pinMode(m0_pin, INPUT); // Z - high impedance
        break;
    }
}
