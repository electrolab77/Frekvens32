# FREQKENS Display Project

This project is a customizable ESP32-based LED matrix display inspired by the IKEA FREKVENS series. It integrates time display, audio pulse synchronization, and scrolling messages. The display is built using a 6x6 pixel font, optimized for compact size and clear visuals.

---

## 🧠 Features

**Real-time Clock (RTC DS3231)**: Displays current time with high accuracy.
**Scrolling Text Messages**: Messages scroll smoothly across the screen with 1-pixel spacing.
**Audio Pulse Sync (PPQN)**: Synchronizes with external audio pulse (e.g. from a synthesizer) using a mini-jack input.
**Mic Input Visualization (MAX4466)**: Detects peaks in ambient sound and reacts accordingly.
**Control Buttons**: 
- **Yellow button**: change display mode.
- **Red button**: manual trigger or reset.
**Power Control via MOSFET Module**: Automatically cuts LED matrix power if ESP32 is not running.

---

## 🔧 Hardware Setup

**Microcontroller**: ESP32 DevKit (3.3 V logic)
**LED Matrix**: Custom 6x6 font over an external 4V-powered matrix
**Power Supply**: IKEA FREKVENS-compatible DC adapter
**MOSFET Switch**: IRFZ44N module to control LED power
**Audio Sync**: PPQN input through a 3.5mm mini-jack
**Microphone Module**: MAX4466 analog mic for peak detection
**RTC Module**: DS3231 (I²C)
**User Input**: Two push-buttons

---

## 🔌 Wiring Diagram

| Component            | Signal/Usage           | ESP32 GPIO / Pin        | Power Source           | GND Connection               | Notes                                        |
|----------------------|-------------------------|--------------------------|-------------------------|------------------------------|----------------------------------------------|
| LED Matrix (SCT)     | Data (IN)               | GPIO 23                  | IKEA 4 V adapter        | GND of external adapter      | Use external 4V, not 3.3 V from ESP32         |
|                      | Clock                   | GPIO 18                  |                         |                              |                                              |
|                      | Latch                   | GPIO 5                   |                         |                              |                                              |
| Audio Mic (MAX4466)  | OUT (analog)            | GPIO 34                  | 3.3 V from ESP32        | GND of ESP32                 | Avoid GPIO 36/39 with Wi-Fi                   |
| RTC DS3231           | SDA (I²C)               | GPIO 21                  | VIN (5 V from ESP32)    | GND of ESP32                 | Compatible with 3.3 V logic                  |
|                      | SCL (I²C)               | GPIO 22                  |                         |                              |                                              |
| Pulse Input (Mini jack) | Signal (PPQN)        | GPIO 35                  | —                       | GND from synth to ESP32 GND  | Use optocoupler or resistor divider          |
| Button - Yellow      | Digital input           | GPIO 17                  | —                       | GND of ESP32                 | Pull-down resistor recommended               |
| Button - Red         | Digital input           | GPIO 16                  | —                       | GND of ESP32                 | Pull-down resistor recommended               |
| MOSFET Module        | Gate (control)          | GPIO 4 (example)         | Controls IKEA 4V VCC    | Common GND (ESP32 + IKEA)    | Turns off LED matrix when ESP32 is off       |
| ESP32 DevKit         | —                       | USB or VIN (5 V input)   |                         | Shared GND                   | Common GND required for all components       |

---

## 🔤 Font System

- 6x6 pixel font stored in `PROGMEM` in `FONT_6X6[]`.
- Each character is defined by 6 bytes (rows).
- Horizontal scrolling with 1 pixel gap between letters.
- Custom drawing function handles row-by-row rendering.

---

## 🖼️ Display Modes

1. **Clock Mode**: HH:MM display using the RTC module.
2. **Pulse Sync Mode**: Animates in sync with incoming PPQN clock pulses.
3. **Mic Mode**: Detects loud sounds and triggers effects.
4. **Text Scroll Mode**: Scrolls a predefined message.

---

## ⚠️ Notes

- All modules must share a **common GND** for stable operation.
- Be sure to use a **logic-compatible MOSFET driver** or module for 3.3 V GPIO control.
- The IKEA FREKVENS adapter delivers 4V—ensure your matrix supports this voltage.
- Avoid using GPIOs 36/39 with Wi-Fi as they may conflict.

---

## 📁 File Structure (suggested)

/src
├── main.cpp
├── display.cpp / display.h
├── font6x6.h
└── config.h

/data
└── messages.txt (optional for scrolling text)

/lib
└── RTClib
└── Adafruit_GFX

README.md
platformio.ini / Arduino project file

---

## 🔄 Future Ideas

- Add Bluetooth/Wi-Fi message update
- Integration with MIDI sync
- Brightness control via ambient light
- Config mode via button + mobile app

---

## 📸 Inspiration

Inspired by the minimalist style of the **IKEA FREKVENS** speaker and light system. This project is designed to be visually simple, responsive to music, and hackable.

---

## 🧾 License

This project is licensed under the MIT License.
Feel free to use, modify, and distribute under the terms of this license.