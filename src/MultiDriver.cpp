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
    long steps[3] = {steps1, steps2, steps3};
    short states[MAX_MOTORS];
    Direction dirs[MAX_MOTORS];
    unsigned long event_timers[MAX_MOTORS];

    /*
     * Initialize state and trigger STEP for all active motors
     */
    enable();
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
        bool done = true;
        FOREACH_MOTOR(
            if (steps[i]){
                done = false;
                if (event_timers[i] < next_event){
                    next_event = event_timers[i];
                }
            }
        );
        if (done){
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
    disable();
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
