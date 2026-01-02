/*
 * TMC2100 Stepper Driver Verification
 * 
 * This example uses the TMC2100 driver.
 */
#include <Arduino.h>
#include "TMC2100.h"

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM 120

// Microstepping mode. If you hardwired it to save pins, set to the same value here.
#define MICROSTEPS 16

#define DIR 8
#define STEP 9
#define SLEEP 13 // optional (just delete SLEEP from constructor if not used)
#define CFG1 10
#define CFG2 11

/*
 * Choose one of the sections below that match your hookup
 */

// Basic connection: only DIR, STEP are connected.
// TMC2100 stepper(MOTOR_STEPS, DIR, STEP);

// Fully wired: DIR, STEP, SLEEP, CFG1, CFG2
TMC2100 stepper(MOTOR_STEPS, DIR, STEP, SLEEP, CFG1, CFG2);

void setup() {
    stepper.begin(RPM, MICROSTEPS);
    // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
    // stepper.setEnableActiveState(LOW);
}

void loop() {
    // energize coils - the motor will hold position
    // stepper.enable();
  
    /*
     * Moving motor one full revolution using the block command
     */
    stepper.rotate(360);
    stepper.rotate(-360);

    delay(1000);

    /*
     * Moving motor to specific microstep levels
     */
    stepper.setMicrostep(1);
    stepper.rotate(360);
    stepper.setMicrostep(16);
    stepper.rotate(360);
}
