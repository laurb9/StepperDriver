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

BasicStepperDriver::BasicStepperDriver(int steps, int dir_pin, int step_pin, int enable_pin)
:motor_steps(steps), dir_pin(dir_pin), step_pin(step_pin), enable_pin(enable_pin)
{
    init();
}

void BasicStepperDriver::init(void){
    pinMode(dir_pin, OUTPUT);
    digitalWrite(dir_pin, HIGH);

    pinMode(step_pin, OUTPUT);
    digitalWrite(step_pin, LOW);

    if IS_CONNECTED(enable_pin){
        pinMode(enable_pin, OUTPUT);
        digitalWrite(enable_pin, HIGH); // disable
    }

    setMicrostep(1);
    setRPM(60); // 60 rpm is a reasonable default

    enable();
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
    for (unsigned ms=1; ms <= this->getMaxMicrostep(); ms<<=1){
        if (microsteps == ms){
            this->microsteps = microsteps;
            break;
        }
    }
    calcStepPulse();
    return this->microsteps;
}

/*
 * Move the motor a given number of steps.
 * positive to move forward, negative to reverse
 */
void BasicStepperDriver::move(long steps){
    unsigned long next_event;
    startMove(steps);
    do {
        next_event = nextAction();
        microWaitUntil(micros() + next_event);
    } while (next_event);
}
/*
 * Move the motor a given number of degrees (1-360)
 */
void BasicStepperDriver::rotate(long deg){
    move(calcStepsForRotation(deg));
}
/*
 * Move the motor with sub-degree precision.
 * Note that using this function even once will add 1K to your program size
 * due to inclusion of float support.
 */
void BasicStepperDriver::rotate(double deg){
    move(calcStepsForRotation(deg));
}

/*
 * Initiate a move (calculate and save the parameters)
 */
void BasicStepperDriver::startMove(long steps){
    dir_state = (steps >= 0) ? HIGH : LOW;
    step_state = LOW;
    steps_remaining = abs(steps);
}
/*
 * Move the motor a given number of degrees (1-360)
 */
void BasicStepperDriver::startRotate(long deg){
    startMove(calcStepsForRotation(deg));
}
/*
 * Move the motor with sub-degree precision.
 * Note that using this function even once will add 1K to your program size
 * due to inclusion of float support.
 */
void BasicStepperDriver::startRotate(double deg){
    startMove(calcStepsForRotation(deg));
}
/*
 * Toggle step and return time until next change is needed (micros)
 */
unsigned long BasicStepperDriver::nextAction(void){
    if (steps_remaining > 0){
        /*
         * DIR pin is sampled on rising STEP edge, so it is set first
         */
        digitalWrite(dir_pin, dir_state);
        if (step_state == LOW){
            step_state = HIGH;
        } else {
            step_state = LOW;
            steps_remaining--;
        }
        digitalWrite(step_pin, step_state);
        /*
         * We currently try to do a 50% duty cycle so it's easy to see.
         * Other option is step_high_min, pulse_duration-step_high_min.
         */
        return step_pulse/2;
    } else {
        return 0; // end of move
    }
}

/*
 * Enable/Disable the motor by setting a digital flag
 */
void BasicStepperDriver::enable(void){
    if IS_CONNECTED(enable_pin){
        digitalWrite(enable_pin, LOW);
    }
}

void BasicStepperDriver::disable(void){
    if IS_CONNECTED(enable_pin){
        digitalWrite(enable_pin, HIGH);
    }
}

unsigned BasicStepperDriver::getMaxMicrostep(){
    return BasicStepperDriver::MAX_MICROSTEP;
}
