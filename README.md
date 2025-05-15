# Frekvens32 Display Project

This project is a customizable ESP32-based LED matrix display inspired by the IKEA FREKVENS series. It integrates time/date display and FX visualization with audio (micro) and pulse (PPQN) synchronization. The display is built using a 6x6 pixel font, optimized for compact size and clear visuals.

---

## 🧠 Features

- **Real-time Clock**: Displays current time/date with NTP synchronization.
- **Audio Pulse Sync (PPQN) Visualization**: Synchronizes with external audio pulse (e.g. from a synthesizer) using a mini-jack input.
- **Mic Input Visualization**: Detects peaks in ambient sound and reacts accordingly.
- **Control Buttons**: Manage display mode and configuration mode with 2 buttons

---

## 🔧 Hardware Setup

- **Microcontroller**: ESP32 DevKit (3.3V)
- **16x16 LED Matrix**: IKEA Frekvens original display (4V)
- **Power Supply**: IKEA Frekvens original 4V power supply
- **Pulse Sync**: PPQN input through a 3.5mm mini-jack
- **Microphone Module**: MAX4466 analog mic for peak detection
- **RTC Module**: DS3231 (I²C)
- **User Input**: IKEA Frekvens original push-buttons (1 yellow + 1 red)

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
| ESP32 DevKit         | —                       | USB or VIN (5 V input)   |                         | Shared GND                   | Common GND required for all components       |

---

## 🔤 Font System

- 6x6 pixel font stored in `PROGMEM` in `FONT_6X6[]`.
- Each character is defined by 6 bytes (rows).
- Horizontal scrolling with 1 pixel gap between letters.
- Custom drawing function handles row-by-row rendering.

---

## 🖼️ Display Modes

1. **Clock Mode**: HHMM, DDMM or YYYY display using the RTC module.
2. **Micro Mode**: Detects loud sounds and triggers effects.
3. **Pulse Mode**: Animates in sync with incoming PPQN clock pulses.

---

## 🕹️ Buttons

- Red short : Start display (if off) / Change mode (normal mode) / Change page (menu mode)
- Red long : Enter the menu (normal mode) / Exit the menu (menu mode)
- Yellow short : Start display (if off) / Change page (normal mode) / Change value (menu mode)
- Yellow long : Save option (menu mode) / Sync NTP (NTP page in menu mode)

---

## 🎛️ Settings Menu

- LED INTENSITY : 10 to 99%
- SCROLL SPEED : Slow (SL), Moderate (MD, default) or Fast (FS)
- TIME FORMAT : 12 or 24 (default)
- SYNC NTP : GO (execute synchronization via WiFi)
- PULSE PPQN : 1, 2, 4, 8, 12, 16, 24 (default), 32, 48, 64 or 96
- CASE TEMP : Temperature display (from RTC)
- ESP32 TEMp : Temperature display

---

## ⚠️ Notes

- All modules must share a **common GND** for stable operation.
- Avoid using GPIOs 36/39 with Wi-Fi as they may conflict.

---

## 💡 Future Ideas

- Add Bluetooth/Wi-Fi message update
- Integration with MIDI sync
- Brightness control via ambient light
- Power Control via MOSFET Module : Automatically cuts LED matrix power if ESP32 is not running.
- Scrolling Text Messages

---

## 📸 Inspiration

Inspired by the minimalist style of the **IKEA Frekvens** speaker and light system. This project is designed to be visually simple, responsive to music, and evolutive.

---

## 🧾 License

This project is licensed under the MIT License.
Feel free to use, modify, and distribute under the terms of this license.