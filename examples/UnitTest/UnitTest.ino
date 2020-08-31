/*
 * This is not an example sketch, it is used to validate code changes 
 * and determine maximum workable RPM/microsteps parameters for a given board.
 * 
 * Usage: run with serial terminal open
 * 
 * All driver tests are done with microstep 1. Increasing microstep halves max rpm with each level.
 * The maximum usable RPM can be determined from the output.
 * The max RPM at a different microstep can be calculated with formula "max rpm / microstep"
 * 
 * Copyright (C)2020 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>

#include "BasicStepperDriver.h"
#include "MultiDriver.h"
#include "SyncDriver.h"

// RPMS contains the list of RPMS to test at, assuming microstep=1
const float RPMS[] = {60, 0.5, 1, 10, 100, 200, 300, 600, 1200, 2400, 4800, 9600};
const int RPMS_COUNT = sizeof(RPMS)/sizeof(*RPMS);
// STEPS is how many steps for each test. More has better accuracy but slower
#define STEPS 20
// ALLOWED_DEVIATION is the error tolerance. 0.10 considers 90% - 110% range aceptable
#define ALLOWED_DEVIATION 0.10

/*
 * Verify that the expected time calculation is correct at different rpms and two microstep levels
 */
bool test_calculations(BasicStepperDriver stepper){
    bool pass = true;
    char t[128];
    for (int i = 0; i < RPMS_COUNT; i++){
        float rpm = RPMS[i];
        for (int microstep = 1; microstep <= 16; microstep <<= 4){
            long expected_micros = long(1000000.0f * 60.0f / rpm);
            stepper.begin(rpm, microstep);
            long estimated_micros = stepper.getTimeForMove(200*microstep);
            sprintf(t, "rpm=%-3d expected=%9uµs estimated %9uµs", 
                    int(rpm), expected_micros, estimated_micros);
            Serial.println(t);
            if (estimated_micros != expected_micros) {
                pass = false;
            }
        }
    }  
    return pass;  
}

/*
 * Pass/fail the result and print it out in a one-line format
 */
bool result(float rpm, int microstep, int steps, long elapsed_micros, long expected_micros){
    bool pass = true;
    char t[128];
    float error = (float(elapsed_micros) - float(expected_micros)) / float(expected_micros);
    unsigned step_micros = expected_micros / steps;
    unsigned error_micros = abs(elapsed_micros - expected_micros) / steps;
    sprintf(t, "microstep=%-3d rpm=%-3d expected=%9uµs elapsed=%9uµs step_err=%4dµs step=%6dµs", 
            microstep, int(rpm), expected_micros, elapsed_micros, error_micros, step_micros);
    Serial.print(t);
    if (error >= ALLOWED_DEVIATION || error <= -ALLOWED_DEVIATION) {
        pass = false;
        Serial.print(" FAIL");
    }
    Serial.println();
    return pass;
}

/*
 * Run the tests for BasicStepperDriver
 */
bool test_basic(BasicStepperDriver stepper){
    bool pass = true;
    for (int i = 0; i < RPMS_COUNT; i++){
        float rpm = RPMS[i];
        stepper.begin(rpm, 1);
        unsigned long start_time_micros = micros();
        stepper.move(STEPS);
        long elapsed_micros = micros() - start_time_micros;
        pass &= result(rpm, 1, STEPS, elapsed_micros, stepper.getTimeForMove(STEPS));
    }
    return pass;
}

/*
 * Run the tests for MultiDriver with 3 motors
 */
bool test_multi(BasicStepperDriver s1, BasicStepperDriver s2, BasicStepperDriver s3){
    MultiDriver controller(s1, s2, s3);
    bool pass = true;
    for (int i = 0; i < RPMS_COUNT; i++){
        float rpm = RPMS[i];
        s1.begin(rpm, 1);
        s2.begin(rpm, 1);
        s3.begin(rpm, 1);
        unsigned long start_time_micros = micros();
        controller.move(STEPS, 2*STEPS/3, -STEPS/2);
        long elapsed_micros = micros() - start_time_micros;
        pass &= result(rpm, 1, STEPS, elapsed_micros, s1.getTimeForMove(STEPS));
    }
    return pass;
}

/*
 * Run the tests for SyncDriver with 3 motors
 */
bool test_sync(BasicStepperDriver s1, BasicStepperDriver s2, BasicStepperDriver s3){
    SyncDriver controller(s1, s2, s3);
    bool pass = true;
    for (int i = 0; i < RPMS_COUNT; i++){
        float rpm = RPMS[i];
        s1.begin(rpm, 1);
        s2.begin(rpm, 1);
        s3.begin(rpm, 1);
        unsigned long start_time_micros = micros();
        controller.move(STEPS, 2*STEPS/3, -STEPS/2);
        long elapsed_micros = micros() - start_time_micros;
        pass &= result(rpm, 1, STEPS, elapsed_micros, s1.getTimeForMove(STEPS));
    }
    return pass;
}

#define TEST_RESULT(result, func, ...) #func "(" #__VA_ARGS__ "): " result
#define RUN_TEST(desc, func, ...) Serial.println(desc); Serial.println(func(__VA_ARGS__) ? TEST_RESULT("OK", func, __VA_ARGS__) : TEST_RESULT("FAIL", func, __VA_ARGS__))

void setup() {

    BasicStepperDriver s1(200, 12, 13);
    BasicStepperDriver s2(200, 12, 13);
    BasicStepperDriver s3(200, 12, 13);

    Serial.begin(1000000);
    delay(2000);
    RUN_TEST("Timing Calculation test", test_calculations, s1);
    RUN_TEST("BasicStepperDriver test", test_basic, s1);
    RUN_TEST("MultiDriver test", test_multi, s1, s2, s3);
    RUN_TEST("SyncDriver test", test_sync, s1, s2, s3);
}

void loop() {
    delay(1);
}
