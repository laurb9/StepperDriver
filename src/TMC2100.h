/*
 * TMC2100 - SilentStepStick Stepper Motor Driver
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef TMC2100_H
#define TMC2100_H
#include <Arduino.h>
#include "BasicStepperDriver.h"

class TMC2100 : public BasicStepperDriver {
protected:
    short cf1_pin = PIN_UNCONNECTED;
    short cf2_pin = PIN_UNCONNECTED;
    // tA STEP minimum, HIGH pulse width (1us)
    static const int step_high_min = 1;
    // tB STEP minimum, LOW pulse width (1us)
    static const int step_low_min = 1;
    // wakeup time, nSLEEP inactive to STEP (1000us)
    static const int wakeup_time = 1000;

    // Get max microsteps supported by the device
    short getMaxMicrostep() override;

private:
    // microstep range (1, 2, 4, 8, 16)
    // maximum level controllable by CFG pins is 1/16, TMC2100 can interpolate to 1/256 internally
    static const short MAX_MICROSTEP = 16;

public:
    /*
     * Basic connection: only DIR, STEP are connected.
     * Microstepping controls should be hardwired.
     */
    TMC2100(short steps, short dir_pin, short step_pin);
    TMC2100(short steps, short dir_pin, short step_pin, short enable_pin);
    
    void begin(float rpm=60, short microsteps=1);
    /*
     * Fully wired. All the necessary control pins for TMC2100 are connected.
     * The CFG pins are used to set microstepping.
     */
    TMC2100(short steps, short dir_pin, short step_pin, short cf1_pin, short cf2_pin);
    TMC2100(short steps, short dir_pin, short step_pin, short enable_pin, short cf1_pin, short cf2_pin);
    short setMicrostep(short microsteps) override;
};
#endif // TMC2100_H
