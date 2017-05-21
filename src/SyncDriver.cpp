/*
 * Synchronous Multi-motor group driver
 * All motors reach their target at the same time.
 *
 * Copyright (C)2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "SyncDriver.h"

#define FOREACH_MOTOR(action) for (short i=count-1; i >= 0; i--){action;}

void SyncDriver::move(long steps1, long steps2, long steps3){
    long steps[3] = {steps1, steps2, steps3};
    long timing[MAX_MOTORS];
    unsigned rpms[MAX_MOTORS];
    /*
     * find which motor would take the longest to finish,
     */
    long m = 0;
    FOREACH_MOTOR(
        long move_time = abs(steps[i]) * motors[i]->getTimePerStep();
        timing[i] = move_time;
        Serial.print(i); Serial.print(" = "); Serial.println(move_time);
        if (move_time > m){
            m = move_time;
        }
    );
    /*
     * Stretch timing for all others by adjusting rpm proportionally
     */
    if (m){
        FOREACH_MOTOR(
            if (steps[i]){
                rpms[i] = motors[i]->getRPM();
                motors[i]->setRPM(rpms[i] * timing[i] / m);
            }
        );
    }
    MultiDriver::move(steps1, steps2, steps3);
    /*
     * Restore original rpm settings
     */
    if (m){
        FOREACH_MOTOR(
            if (rpms[i]){
                motors[i]->setRPM(rpms[i]);
            }
        );
    }
}
