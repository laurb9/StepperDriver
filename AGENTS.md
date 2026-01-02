# AGENTS.md - AI Coding Assistant Guidelines

This file provides guidelines for AI coding assistants working with the StepperDriver repository.

## Project Overview

**StepperDriver** is an Arduino library for controlling stepper motors via driver boards that use STEP/DIR pins (indexer mode). It supports multiple driver ICs including A4988, DRV8825, DRV8834, and DRV8880.

- **Language**: C++ (Arduino)
- **License**: MIT
- **Target Platforms**: Arduino (AVR, SAM), ESP8266, ESP32, Teensy
- **Author/Maintainer**: Laurentiu Badea

## Repository Structure

```
StepperDriver/
├── src/                    # Library source code
│   ├── BasicStepperDriver.h/cpp  # Base driver class
│   ├── A4988.h/cpp              # A4988-specific driver
│   ├── DRV8825.h/cpp            # DRV8825-specific driver
│   ├── DRV8834.h/cpp            # DRV8834-specific driver
│   ├── DRV8880.h/cpp            # DRV8880-specific driver
│   ├── MultiDriver.h/cpp        # Multi-motor coordination
│   └── SyncDriver.h/cpp         # Synchronized multi-motor movement
├── examples/               # Arduino example sketches
├── test/                   # PlatformIO Unit Test files
├── qemu/                   # QEMU emulation support (experimental)
├── .github/workflows/      # CI workflows (Arduino + PlatformIO)
├── library.properties      # Arduino library metadata
├── library.json            # PlatformIO library metadata
├── platformio.ini          # PlatformIO configuration
└── Makefile                # Build automation for arduino-cli
```

## Class Hierarchy

```
BasicStepperDriver        # Base class - generic 2-pin (DIR/STEP) control
├── A4988                 # Adds microstepping control (MS1/MS2/MS3)
│   └── DRV8825          # Extends A4988 with M0/M1/M2 and 1:32 support
│       └── DRV8834      # Low-voltage variant, M0/M1 only
│           └── DRV8880  # Adds current/torque control
└── MultiDriver           # Coordinates multiple motors
    └── SyncDriver       # Synchronized movement for multiple motors
```

## Coding Conventions

### Style Guidelines
- Use 4-space indentation
- Function and variable names use `camelCase` for public API, `snake_case` for internal variables
- Class names use `PascalCase`
- Constants and macros use `UPPER_SNAKE_CASE`
- Include Arduino.h in all headers that use Arduino types

### Header File Structure
- Use include guards: `#ifndef HEADER_NAME_H` / `#define HEADER_NAME_H`
- Include copyright notice with MIT license reference
- Group members: public first, then protected, then private

### Important Patterns
- `PIN_UNCONNECTED (-1)` marks optional, unused pins - always check with `IS_CONNECTED(pin)`
- Motor steps are typically 200 (1.8° motors) or 400 (0.9° motors)
- Microstepping values must be powers of 2 (1, 2, 4, 8, 16, 32)
- `enable()` and `disable()` control the ~SLEEP/~ENABLE pin states

## Building and Testing

This project supports multiple build systems. 

### Using Arduino CLI via Makefile
```bash
# Initial setup (downloads arduino-cli and configures environment)
make setup

# Build all examples for Arduino Uno
make all TARGET=arduino:avr:uno

# Build a specific example
make BasicStepperDriver.hex TARGET=arduino:avr:uno

# Clean build artifacts
make clean
```

### Using PlatformIO

```bash
# Initial setup (downloads PlatformIO)
pip install platformio

# Build for default boards
platformio run

# Build for specific board
platformio run -e esp32dev
platformio run -e uno

# Run unit tests if any
platformio test

# Run examples
platformio ci --lib src --keep-build-dir --board esp32dev examples/BasicStepperDriver 
```

### QEMU Testing (Experimental)

```bash
# Build and run in emulator
make BasicStepperDriver.hex TARGET=arduino:avr:uno
make -C qemu qemu-run EXAMPLE=BasicStepperDriver TARGET=arduino:avr:uno
```

## CI/CD

Pull requests and pushes to `master`/`develop` trigger:
- **Arduino workflow**: Builds all examples for AVR, SAMD, ESP8266 against Arduino framework
- **PlatformIO workflow**: Builds for ESP32, ESP8266, Teensy, SAMD, AVR

All CI must pass before merging PRs.

## Commit & Pull Request Guidelines

- **Commit messages**: Use clear, descriptive titles summarizing changes; reference issue numbers where applicable
- **Pull requests**: Include a brief description of the feature or fix, any relevant test results, and performance impact on which platform, if any
- **Signature**: Please add a footer to your commit message indicating your identity, e.g., `Agent: gemini-2.0-pro (antigravity)` or `Agent: claude-3.5-sonnet (cursor)`
- **Branch naming**: Create feature branches from main with descriptive names (e.g., feature/new-driver, fix/timeout-issue)
- All PRs should pass CI checks before merge

## Key Technical Details

### Hardware
- Supports stepper driver boards that use STEP/DIR pins (indexer mode) and microstepping control
- Must respect driver parameters such as microstep configuration, signal timing, etc. as documented in the driver chip datasheet

### Timing Constraints
- `step_high_min` / `step_low_min`: Minimum pulse durations (μs) - driver-specific
- `wakeup_time`: Time after enabling before movement (μs)
- High RPM + high microstepping = limited by MCU speed

### Movement Modes
- `CONSTANT_SPEED`: Fixed RPM throughout movement
- `LINEAR_SPEED`: Acceleration/deceleration with configurable rates (steps/s²)

### Blocking vs Non-Blocking
- Blocking: `move()`, `rotate()` - return when complete
- Non-blocking: `startMove()`, `startRotate()` + `nextAction()` loop

## Performance Constraints

> **Critical**: This library targets 8-bit and 16-bit microcontrollers (ATmega328P, ATtiny, etc.) with severely limited resources. All code changes must account for these constraints.

### Target Platform Limitations
- **RAM**: As low as 2KB (ATmega328P) - avoid dynamic allocation, large buffers, or deep call stacks
- **Flash**: As low as 32KB - minimize code size, avoid template bloat
- **CPU Speed**: 8-20 MHz typical - every cycle counts in timing-critical paths
- **No FPU**: Floating-point operations are emulated in software on 8-bit MCUs (slow and large). ESP32 and similar MCUs do have FPU but we target a lowest common denominator.

### Step Loop Timing
The `nextAction()` method and related step pulse generation code are **timing-critical hot paths**. At high RPM with microstepping, step pulses may need to fire at 10,000+ Hz. In this code:

- **Avoid function calls** where inline code suffices
- **Avoid floating-point math** - use integer/fixed-point arithmetic
- **Avoid divisions** - they're slow on AVR; use bit shifts for powers of 2
- **Minimize conditional branches** - keep the fast path straight
- **Don't add debug/logging code** that runs during stepping

### Memory Considerations
- Prefer `short` or `int` over `long` when range permits
- Use `const` and `PROGMEM` for constant data where appropriate
- Avoid `String` class - use C-strings with fixed buffers if needed
- Don't allocate memory with `new`/`malloc` in library code

### What This Means for Changes
Before modifying timing-critical code paths:
1. Consider if the change adds cycles to the step loop
2. Test on an 8-bit AVR board (Arduino Uno), not just faster ARM/ESP boards
3. Verify high-RPM operation still works (e.g., 400+ RPM at 1:128 microstepping)
4. Estimate the new maximum RPM across the target MCUs and update the documentation.

## Common Tasks for AI Assistants

### Adding a New Driver
1. Find and read the datasheet for the driver chip
2. Create new `.h` and `.cpp` files in `src/`
3. Inherit from appropriate base class (typically `A4988` or `DRV8825`)
4. Override `getMaxMicrostep()` if different from parent
5. Override `setMicrostep()` if pin configuration differs
6. Update timing constants if needed (`step_high_min`, `wakeup_time`)
7. Add to `keywords.txt` for Arduino IDE highlighting
8. Create an example sketch in `examples/`

### Modifying Timing Calculations
- Key formula in header: `STEP_PULSE(steps, microsteps, rpm)`
- Acceleration math is in `BasicStepperDriver::calcStepPulse()`
- Be careful with integer overflow on slower MCUs

### Adding Examples
1. Create a new directory under `examples/`
2. Add a `.ino` file with the same name as the directory
3. Include required headers (e.g., `#include "A4988.h"`)
4. Use standard Arduino pin definitions for portability

## Dependencies

This library has no external dependencies - it only requires the Arduino core library (`Arduino.h`).

## Testing Changes

Before submitting changes:
1. Build examples for multiple boards (at least AVR and one ARM-based)
2. Verify the library compiles without warnings (`--warnings all`)
3. If modifying timing-critical code, review the generated asm code to ensure it's optimized.
4. If modifying timing, ask for a test with actual hardware if possible
5. Check that changes don't break backward compatibility
