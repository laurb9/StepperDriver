---
description: Build the project and run tests to ensure code integrity
---

1. Check for PlatformIO and install if missing
   if ! command -v platformio &> /dev/null; then pip install platformio; fi

2. Build all examples using PlatformIO (covers multiple architectures)
   // turbo
   platformio run

3. Build using Arduino CLI (covers AVR)
   // turbo
   make all TARGET=arduino:avr:uno

4. Run examples (covers all examples)
   // turbo
   for sketch in examples/*; do
     platformio ci --lib src --keep-build-dir --board esp32dev ${sketch}
   done
