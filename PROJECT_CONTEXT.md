# Cat Deterrent 2000 - Project Context

## Purpose

Build a small, expandable device that discourages a cat from approaching
houseplants. The first version detects nearby movement and plays a hissing sound.

This file is the handoff document for future Codex/ChatGPT conversations. Read it
before proposing changes, and update it whenever the hardware, wiring, design, or
project status changes.

## Owner and Working Style

- The owner is new to Arduino and embedded development.
- Work in small, testable increments.
- Explain wiring clearly before power is applied.
- Test one component at a time so failures are easy to isolate.
- Prefer maintainable code and non-blocking timing when practical.
- Do not jump ahead to camera or AI features until the basic deterrent is stable.

## Version 1 Goal

```text
Cat moves near plant
        |
        v
PIR detects movement
        |
        v
Arduino receives signal
        |
        v
DFPlayer Mini plays hiss.mp3
        |
        v
10-second cooldown
        |
        v
System is ready again
```

## Hardware on Hand

- Arduino Uno (USB identity and successful upload confirmed)
- HC-SR501 PIR motion sensor
- DFPlayer Mini MP3 module
- Speaker
- microSDHC card
- ELEGOO 0.96-inch, 128x64, yellow/blue I2C OLED display
- Breadboard, jumper wires, and supporting kit components

## Current Technical Decisions

- Development environment: Visual Studio Code with PlatformIO
- Framework: Arduino
- Confirmed PlatformIO environment: `atmelavr` platform with `uno` board
- Current USB upload and monitor port: COM3
- OLED controller family: SSD1306
- OLED I2C address: `0x3C` (confirmed with an I2C scan)
- OLED wiring:
  - GND -> Uno GND
  - VCC -> Uno 5V
  - SDA -> Uno A4
  - SCL -> Uno A5
- OLED libraries:
  - `adafruit/Adafruit SSD1306`
  - `adafruit/Adafruit GFX Library`
- The earlier GPIO 14 suggestion was based on an incorrect ESP32 assumption and
  must not be used as Uno wiring guidance.
- Serial monitor speed: 115200 baud
- Audio storage: microSDHC formatted as FAT32
- Recommended card capacity: 16 GB or 32 GB
- Initial sound filename: `hiss.mp3`
- Initial cooldown after activation: 10 seconds

Confirm module markings, voltage requirements, and pinout before connecting
components.

## Electrical and Reliability Notes

- All modules controlled by the Uno must share a common ground.
- Confirm each module's supply voltage before connecting power.
- Arduino Uno GPIO uses 5 V logic. Check compatibility before connecting any
  future 3.3 V-only module.
- Verify the DFPlayer Mini serial connection and speaker requirements before
  wiring it to the Uno.
- Power down the circuit before moving wires.
- The PIR sensor may require a short warm-up period and may trigger while it
  stabilizes.
- Use a cooldown/state-machine approach to prevent continuous sound playback.

## Planned Build Sequence

1. ~~Identify the controller and confirm its PlatformIO target.~~ Confirmed as
   Arduino Uno using the `uno` target.
2. ~~Create the PlatformIO project and verify that it builds.~~ Completed.
3. ~~Upload a minimal blink and serial-output program.~~ Completed.
4. ~~Confirm serial output at 115200 baud in PlatformIO Monitor.~~ Completed.
5. ~~Wire the OLED and confirm its I2C address.~~ Completed; address is `0x3C`.
6. Upload the current OLED firmware and visually confirm the displayed text.
7. Choose and document an Uno digital pin for the PIR signal.
8. Connect only the PIR sensor and observe motion events in the serial monitor.
9. Prepare and test the microSD card and DFPlayer Mini separately.
10. Integrate motion detection with hiss playback.
11. Add a 10-second, preferably non-blocking cooldown.
12. Test placement, sensitivity, false triggers, volume, and power stability.
13. Document the final wiring and operating instructions.

## Current Status

- Parts have been acquired.
- The connected controller was identified as an Arduino Uno on COM3.
- PlatformIO Core 6.1.19 and the Atmel AVR platform are installed.
- The project builds successfully in PlatformIO.
- A blink-and-serial firmware test was uploaded and flash-verified successfully
  on July 30, 2026.
- Serial Monitor output was confirmed at 115200 baud.
- The OLED was wired to the Uno and an I2C scanner found it at `0x3C`.
- The Adafruit SSD1306 and GFX libraries were added to `platformio.ini`.
- `src/main.cpp` initializes the OLED and uses a reusable
  `DisplayText(const char* text)` function.
- The current OLED firmware builds successfully. Its upload and visible output
  have not yet been recorded as confirmed.
- Current build memory use: 655 bytes RAM (32.0%) and 15,184 bytes flash (47.1%).
- The PIR sensor and audio components have not been wired or tested.

## Immediate Next Step

Upload the current OLED firmware and confirm that the display shows the heading
and connection message. If successful, simplify the test message as desired,
then choose and document the Uno digital pin that will receive the PIR sensor's
output before wiring it.

## Future Ideas

Only consider these after Version 1 works reliably:

- Wi-Fi configuration and web dashboard
- Detection counts and statistics
- Adjustable cooldown and operating schedule
- Battery backup
- Mobile notifications
- Home Assistant integration
- Camera-based filtering
- AI recognition of the specific cat

The previously preferred camera candidate was the Seeed Studio XIAO ESP32-S3
Sense, but camera hardware should be reevaluated when that phase begins.

## Handoff Prompt for a New Chat

Use this prompt when starting a new project conversation:

> Please read `PROJECT_CONTEXT.md` first. Continue the Cat Deterrent 2000 project
> from its recorded current status. Work one verified step at a time, update the
> context file when decisions or progress change, and do not assume unconfirmed
> board or wiring details.

## Maintenance Rule

After each meaningful milestone, update at least:

- **Current Status**
- **Immediate Next Step**
- Any changed wiring, pins, dependencies, or design decisions

Record only completed tests as completed. Keep planned work separate from
verified results.
