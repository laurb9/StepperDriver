/*
 * Generic Stepper Motor Driver Driver
 * Indexer mode only.
 *
 * Copyright (C)2015-2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef STEPPER_DRIVER_BASE_H
#define STEPPER_DRIVER_BASE_H
#include <Arduino.h>

// used internally by the library to mark unconnected pins
#define PIN_UNCONNECTED -1
#define IS_CONNECTED(pin) (pin != PIN_UNCONNECTED)

enum Direction {DIR_FORWARD, DIR_REVERSE};

/*
 * calculate the step pulse in microseconds for a given rpm value.
 * 60[s/min] * 1000000[us/s] / microsteps / steps / rpm
 */
#define STEP_PULSE(steps, microsteps, rpm) (60*1000000L/steps/microsteps/rpm)

inline void microWaitUntil(unsigned long target_micros){
    yield();
    while (micros() < target_micros);
}
#define DELAY_MICROS(us) microWaitUntil(micros() + us)

/*
 * Basic Stepper Driver class.
 * Microstepping level should be externally controlled or hardwired.
 */
class BasicStepperDriver {
protected:
    int motor_steps;
    int rpm = 60;
    int dir_pin;
    int step_pin;
    int enable_pin = PIN_UNCONNECTED;

    /*
     * Movement state
     */
    // how many steps are left to complete the current move (absolute value)
    long steps_remaining;
    // DIR pin state
    short dir_state;
    // STEP pin state (HIGH / LOW)
    short step_state = LOW;
    // microseconds until next state change
    unsigned long next_pulse_time;

    // current microstep level, must be < getMaxMicrostep()
    // for 1:16 microsteps is 16
    unsigned microsteps = 1;
    // step pulse duration (microseconds), depends on rpm and microstep level
    unsigned long step_pulse;

    void init(void);
    void calcStepPulse(void);

    // tWH(STEP) pulse duration, STEP high, min value (us)
    static const int step_high_min = 1;
    // tWL(STEP) pulse duration, STEP low, min value (us)
    static const int step_low_min = 1;
    // tWAKE wakeup time, nSLEEP inactive to STEP (us)
    static const int wakeup_time = 0;

    // Get max microsteps supported by the device
    virtual unsigned getMaxMicrostep();

private:
    // microstep range (1, 16, 32 etc)
    static const unsigned MAX_MICROSTEP = 128;

public:
    /*
     * Basic connection: DIR, STEP are connected.
     */
    BasicStepperDriver(int steps, int dir_pin, int step_pin);
    BasicStepperDriver(int steps, int dir_pin, int step_pin, int enable_pin);
    /*
     * Set current microstep level, 1=full speed, 32=fine microstepping
     * Returns new level or previous level if value out of range
     */
    unsigned setMicrostep(unsigned microsteps);
    /*
     * Move the motor a given number of steps.
     * positive to move forward, negative to reverse
     */
    void move(long steps);
    /*
     * Rotate the motor a given number of degrees (1-360)
     */
    void rotate(long deg);
    inline void rotate(int deg){
        rotate((long)deg);
    };
    /*
     * Rotate using a float or double for increased movement precision.
     */
    void rotate(double deg);
    /*
     * Set target motor RPM (1-200 is a reasonable range)
     */
    void setRPM(unsigned rpm);
    unsigned getRPM(void){ return rpm; };
    /*
     * Turn off/on motor to allow the motor to be moved by hand/hold the position in place
     */
    void enable(void);
    void disable(void);
    /*
     * Methods to allow external timing control.
     * These should not be needed for normal use.
     */
    /*
     * Initiate a move (calculate and save the parameters)
     */
    void startMove(long steps);
    inline void startRotate(int deg){ startRotate((long)deg); };
    void startRotate(long deg);
    void startRotate(double deg);
    /*
     * Toggle step and return time until next change is needed (micros)
     */
    unsigned long nextAction(void);

    unsigned long getTimePerStep(void){
        return step_pulse;
    }
    /*
     * Calculate steps needed to rotate requested angle, given in degrees
     */
    long calcStepsForRotation(long deg){
        return deg * motor_steps * (long)microsteps / 360;
    }
    long calcStepsForRotation(double deg){
        return deg * motor_steps * microsteps / 360;
    }
};
#endif // STEPPER_DRIVER_BASE_H
