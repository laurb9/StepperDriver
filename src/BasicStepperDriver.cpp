/*
 * Generic Stepper Motor Driver Driver
 * Indexer mode only.

 * Copyright (C)2015-2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 *
 * Linear speed profile calculations based on
 * - Generating stepper-motor speed profiles in real time - David Austin, 2004
 * - Atmel AVR446: Linear speed control of stepper motor, 2006
 */
#include "BasicStepperDriver.h"

/*
 * Basic connection: only DIR, STEP are connected.
 * Microstepping controls should be hardwired.
 */
BasicStepperDriver::BasicStepperDriver(int steps, int dir_pin, int step_pin)
:motor_steps(steps), dir_pin(dir_pin), step_pin(step_pin)
{}

BasicStepperDriver::BasicStepperDriver(int steps, int dir_pin, int step_pin, int enable_pin)
:motor_steps(steps), dir_pin(dir_pin), step_pin(step_pin), enable_pin(enable_pin)
{}

/*
 * Initialize pins, calculate timings etc
 */
void BasicStepperDriver::begin(int rpm, unsigned microsteps){
    pinMode(dir_pin, OUTPUT);
    digitalWrite(dir_pin, HIGH);

    pinMode(step_pin, OUTPUT);
    digitalWrite(step_pin, LOW);

    if IS_CONNECTED(enable_pin){
        pinMode(enable_pin, OUTPUT);
        digitalWrite(enable_pin, HIGH); // disable
    }

    setMicrostep(microsteps);
    setRPM(rpm);

    enable();
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
    for (unsigned ms=1; ms <= getMaxMicrostep(); ms<<=1){
        if (microsteps == ms){
            this->microsteps = microsteps;
            break;
        }
    }
    calcStepPulse();
    return this->microsteps;
}

/*
 * Set speed profile - CONSTANT_SPEED, LINEAR_SPEED (accelerated)
 */
void BasicStepperDriver::setSpeedProfile(Mode mode, unsigned accel, unsigned decel){
    this->mode = mode;
    this->accel = accel;
    this->decel = decel;
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
 * Set up a new move (calculate and save the parameters)
 */
void BasicStepperDriver::startMove(long steps){
    unsigned long speed;
    dir_state = (steps >= 0) ? HIGH : LOW;
    step_state = LOW;
    steps_remaining = abs(steps);
    step_count = 0;
    switch (mode){
        case LINEAR_SPEED:
            // speed is in [steps/s]
            speed = rpm * motor_steps * microsteps / 60;
            // how many steps from 0 to target rpm
            steps_to_cruise = speed * speed / (2 * accel * microsteps);
            // how many steps from 0 til we need to begin slowing down
            steps_to_brake = steps_remaining * decel / (accel + decel);
            if (steps_to_cruise < steps_to_brake){
                // will reach max speed before needing to brake
                steps_to_brake = steps_to_cruise * accel / decel;
            } else {
                // cannot reach max speed, will need to brake early
                steps_to_cruise = steps_to_brake;
                steps_to_brake = steps_remaining - steps_to_cruise;
            }
            // Initial pulse (c0) including error correction factor 0.676 [us]
            step_pulse = (1e+6)*0.676*sqrt(2.0f/(accel*microsteps));
            break;
        case CONSTANT_SPEED:
        default:
            step_pulse = STEP_PULSE(rpm, motor_steps, microsteps);
    }
}
/*
 * Return calculated time to complete the given move
 */
unsigned long BasicStepperDriver::getTimeForMove(long steps){
    unsigned long t;
    switch (mode){
        case LINEAR_SPEED:
            startMove(steps);
            t = sqrt(2 * steps_to_cruise / accel) + 
                (steps_remaining - steps_to_cruise - steps_to_brake) * STEP_PULSE(rpm, motor_steps, microsteps) +
                sqrt(2 * steps_to_brake / decel);
            break;
        case CONSTANT_SPEED:
        default:
            t = step_pulse * steps_remaining;
    }
    return t;
}
/*
 * Move the motor an integer number of degrees (360 = full rotation)
 * This has poor precision for small amounts, since step is usually 1.8deg
 */
void BasicStepperDriver::startRotate(long deg){
    startMove(calcStepsForRotation(deg));
}
/*
 * Move the motor with sub-degree precision.
 * Note that calling this function will increase program size substantially
 * due to inclusion of float support.
 */
void BasicStepperDriver::startRotate(double deg){
    startMove(calcStepsForRotation(deg));
}

/*
 * calculate the interval til the next pulse
 */
void BasicStepperDriver::calcStepPulse(void){
    // feed remainder into successive steps to increase accuracy (Atmel DOC8017)
    static long rest = 0;
    // if constant speed
    if (mode == LINEAR_SPEED){
        if (step_count <= steps_to_cruise){
            /*
             * accelerating
             */
            step_pulse = step_pulse - (2*step_pulse+rest)/(4*step_count+1);
            rest = (step_count < steps_to_cruise) ? (2*step_pulse+rest) % (4*step_count+1) : 0;
        } else if (steps_remaining > steps_to_brake){
            /*
             * cruising (no speed changes)
             */
        } else {
            /*
             * decelerating
             */
            step_pulse = step_pulse - (2*step_pulse+rest)/(-4*steps_remaining+1);
            rest = (2*step_pulse+rest) % (-4*steps_remaining+1);
        }
    }
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
            step_count++;
            calcStepPulse();
        }
        digitalWrite(step_pin, step_state);
        /*
         * We currently try to do a 50% duty cycle so it's easy to see.
         * Other option is step_high_min, pulse_duration-step_high_min.
         */
        return step_pulse/2; // FIXME: precision loss (1us)
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
