/*
 * Multi-motor group driver
 *
 * Copyright (C)2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "MultiDriver.h"

#define FOREACH_MOTOR(action) for (short i=count-1; i >= 0; i--){action;}

/*
 * Move each motor the requested number of steps, in parallel
 * positive to move forward, negative to reverse, 0 to remain still
 */
void MultiDriver::move(long steps1, long steps2, long steps3){
    steps[0] = steps1;
    steps[1] = steps2;
    steps[2] = steps3;

    /*
     * Initialize state and trigger STEP for all active motors
     */
    FOREACH_MOTOR(
        if (steps[i]){
            dirs[i] = (steps[i] > 0) ? DIR_FORWARD : DIR_REVERSE;
            steps[i] = abs(steps[i]);
            states[i] = HIGH;
            event_timers[i] = motors[i]->step(HIGH, dirs[i]);
        }
    );

    while (true){
        // Find the time when the next pulse needs to fire
        // this is the smallest timer value from all active motors
        unsigned long next_event = ~0L;
        ready = true;
        FOREACH_MOTOR(
            if (steps[i]){
                ready = false;
                if (event_timers[i] < next_event){
                    next_event = event_timers[i];
                }
            }
        );
        if (ready){
            break;
        }

        // wait until the next event
        microWaitUntil(micros() + next_event);

        FOREACH_MOTOR(
            if (steps[i]){
                if (event_timers[i] == next_event){ // motor ready for next action

                    // Toggle STEP and set timer for next pulse
                    states[i] = (states[i] == LOW) ? HIGH : LOW;
                    event_timers[i] = motors[i]->step(states[i], dirs[i]);

                    // If final state is LOW, decrement remaining steps for this motor
                    if (states[i] == LOW){
                        steps[i] -= 1;
                    }
                } else {
                    // Reduce all other event timers by the current left time
                    event_timers[i] -= next_event;
                };
            }
        );
    }
}

#define CALC_STEPS(i, deg) ((motors[i] && deg) ? motors[i]->calcStepsForRotation(deg) : 0)
void MultiDriver::rotate(long deg1, long deg2, long deg3){
    move(CALC_STEPS(0, deg1), CALC_STEPS(1, deg2), CALC_STEPS(2, deg3));
}

void MultiDriver::rotate(double deg1, double deg2, double deg3){
    move(CALC_STEPS(0, deg1), CALC_STEPS(1, deg2), CALC_STEPS(2, deg3));
}

void MultiDriver::setMicrostep(unsigned microsteps){
    FOREACH_MOTOR(motors[i]->setMicrostep(microsteps));
}

void MultiDriver::enable(void){
    FOREACH_MOTOR(motors[i]->enable());
}
void MultiDriver::disable(void){
    FOREACH_MOTOR(motors[i]->disable());
}
