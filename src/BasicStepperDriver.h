/*
 * Generic Stepper Motor Driver Driver
 * Indexer mode only.
 *
 * Copyright (C)2015-2018 Laurentiu Badea
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

/*
 * calculate the step pulse in microseconds for a given rpm value.
 * 60[s/min] * 1000000[us/s] / microsteps / steps / rpm
 */
#define STEP_PULSE(steps, microsteps, rpm) (60.0*1000000L/steps/microsteps/rpm)

// don't call yield if we have a wait shorter than this
#define MIN_YIELD_MICROS 50

/*
 * Basic Stepper Driver class.
 * Microstepping level should be externally controlled or hardwired.
 */
class BasicStepperDriver {
public:
    enum Mode {CONSTANT_SPEED, LINEAR_SPEED};
    enum State {STOPPED, ACCELERATING, CRUISING, DECELERATING};
    struct Profile {
        Mode mode = CONSTANT_SPEED;
        short accel = 1000;     // acceleration [steps/s^2]
        short decel = 1000;     // deceleration [steps/s^2]    
    };
    static inline void delayMicros(unsigned long delay_us, unsigned long start_us = 0){
        if (delay_us){
            if (!start_us){
                start_us = micros();
            }
            if (delay_us > MIN_YIELD_MICROS){
                yield();
            }
            // See https://www.gammon.com.au/millis
            while (micros() - start_us < delay_us);
        }
    }

private:
    // calculation remainder to be fed into successive steps to increase accuracy (Atmel DOC8017)
    long rest;
    unsigned long last_action_end = 0;
    unsigned long next_action_interval = 0;

protected:
    /*
     * Motor Configuration
     */
    short motor_steps;           // motor steps per revolution (usually 200)

    /*
     * Driver Configuration
     */
    short dir_pin;
    short step_pin;
    short enable_pin = PIN_UNCONNECTED;
    short enable_active_state = HIGH;
    // Get max microsteps supported by the device
    virtual short getMaxMicrostep();
    // current microstep level (1,2,4,8,...), must be < getMaxMicrostep()
    short microsteps = 1;
    // tWH(STEP) pulse duration, STEP high, min value (us)
    static const int step_high_min = 1;
    // tWL(STEP) pulse duration, STEP low, min value (us)
    static const int step_low_min = 1;
    // tWAKE wakeup time, nSLEEP inactive to STEP (us)
    static const int wakeup_time = 0;

    float rpm = 0;

    /*
     * Movement state
     */
    struct Profile profile;

    long step_count;        // current position
    long steps_remaining;   // to complete the current move (absolute value)
    long steps_to_cruise;   // steps to reach cruising (max) rpm
    long steps_to_brake;    // steps needed to come to a full stop
    long step_pulse;        // step pulse duration (microseconds)
    long cruise_step_pulse; // step pulse duration for constant speed section (max rpm)

    // DIR pin state
    short dir_state;

    void calcStepPulse(void);

    // this is internal because one can call the start methods while CRUISING to get here
    void alterMove(long steps);

private:
    // microstep range (1, 16, 32 etc)
    static const short MAX_MICROSTEP = 128;

public:
    /*
     * Basic connection: DIR, STEP are connected.
     */
    BasicStepperDriver(short steps, short dir_pin, short step_pin);
    BasicStepperDriver(short steps, short dir_pin, short step_pin, short enable_pin);
    /*
     * Initialize pins, calculate timings etc
     */
    void begin(float rpm=60, short microsteps=1);
    /*
     * Set current microstep level, 1=full speed, 32=fine microstepping
     * Returns new level or previous level if value out of range
     */
    virtual short setMicrostep(short microsteps);
    short getMicrostep(void){
        return microsteps;
    }
    short getSteps(void){
        return motor_steps;
    }
    /*
     * Set target motor RPM (1-200 is a reasonable range)
     */
    void setRPM(float rpm);
    float getRPM(void){
        return rpm;
    };
    float getCurrentRPM(void){
        return (60.0*1000000L / step_pulse / microsteps / motor_steps);
    }
    /*
     * Set speed profile - CONSTANT_SPEED, LINEAR_SPEED (accelerated)
     * accel and decel are given in [full steps/s^2]
     */
    void setSpeedProfile(Mode mode, short accel=1000, short decel=1000);
    void setSpeedProfile(struct Profile profile);
    struct Profile getSpeedProfile(void){
        return profile;
    }
    short getAcceleration(void){
        return profile.accel;
    }
    short getDeceleration(void){
        return profile.decel;
    }
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
     * Configure which logic state on ENABLE pin means active
     * when using SLEEP (default) this is active HIGH
     */
    void setEnableActiveState(short state);
    /*
     * Turn off/on motor to allow the motor to be moved by hand/hold the position in place
     */
    virtual void enable(void);
    virtual void disable(void);
    /*
     * Methods for non-blocking mode.
     * They use more code but allow doing other operations between impulses.
     * The flow has two parts - start/initiate followed by looping with nextAction.
     * See NonBlocking example.
     */
    /*
     * Initiate a move over known distance (calculate and save the parameters)
     * Pick just one based on move type and distance type.
     * If time (microseconds) is given, the driver will attempt to execute the move in exactly that time
     * by altering rpm for this move only (up to preset rpm).
     */
    void startMove(long steps, long time=0);
    inline void startRotate(int deg){
        startRotate((long)deg);
    };
    void startRotate(long deg);
    void startRotate(double deg);
    /*
     * Toggle step at the right time and return time until next change is needed (micros)
     */
    long nextAction(void);
    /*
     * Optionally, call this to begin braking (and then stop) early
     * For constant speed, this is the same as stop()
     */
    void startBrake(void);
    /*
     * Immediate stop
     * Returns the number of steps remaining.
     */
    long stop(void);
    /*
     * State querying
     */
    enum State getCurrentState(void);
    /*
     * Get the number of completed steps so far.
     * This is always a positive number
     */
    long getStepsCompleted(void){
        return step_count;
    }
    /*
     * Get the number of steps remaining to complete the move
     * This is always a positive number
     */
    long getStepsRemaining(void){
        return steps_remaining;
    }
    /*
     * Get movement direction: forward +1, back -1
     */
    int getDirection(void){
        return (dir_state == HIGH) ? 1 : -1;
    }
    /*
     * Return calculated time to complete the given move
     */
    long getTimeForMove(long steps);
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
