# Frekvens32 Display Project

This project is a customizable ESP32-based LED matrix display inspired by the IKEA FREKVENS series. It integrates time/date display and FX visualization free, audio or pulse (PPQN) synchronization. The display is built using a 6x6 pixel font, optimized for compact size and clear visuals.

---

## 🧠 Features

- **Real-time Clock**: Displays current time/date with NTP synchronization.
- **Free Visualization**: Free running display effects (no sync).
- **Mic Input Visualization**: Detects peaks in ambient sound and reacts accordingly.
- **Audio Pulse Sync (PPQN) Visualization**: Synchronizes with external audio pulse (e.g. from a synthesizer) using a mini-jack input. **Not yet implemented**
- **Control Buttons**: Manage display mode and configuration mode with 2 buttons

---

## 🔧 Hardware Setup

- **Microcontroller**: ESP32 DevKit (3.3V)
- **16x16 LED Matrix**: IKEA Frekvens original display (~4V)
- **3.5mm mini-jack**: PPQN Pulse Sync 
- **Microphone Module**: MAX4466 analog mic for peak detection
- **RTC Module**: DS3231 (I²C)
- **3.7V DC/DC Buck Converter**: Convert the ESP32 5V output to 3.7V for LED matrix 
- **User Input**: IKEA Frekvens original push-buttons (1 yellow + 1 red)

---

## 🔌 Wiring Diagram

| Component               | Signal/Usage            | ESP32 GPIO/PIN           | Power Source            | GND Connection               | Notes                                        |
|-------------------------|-------------------------|--------------------------|-------------------------|------------------------------|----------------------------------------------|
| LED Matrix (SCT)        | Data (IN)               | GPIO 23                  | VIN (5V from ESP32)     | GND of ESP32                 | Or original IKEA's PS but avoid ESP32 3.3V   |
|                         | Clock                   | GPIO 18                  |                         |                              |                                              |
|                         | Latch                   | GPIO 5                   |                         |                              |                                              |
| Audio Mic (MAX4466)     | OUT (analog)            | GPIO 34                  | 3.3V from ESP32         | GND of ESP32                 | Avoid GPIO 36/39 with Wi-Fi                  |
| RTC DS3231              | SDA (I²C)               | GPIO 21                  | VIN (5V from ESP32)     | GND of ESP32                 | Compatible with 3.3V logic                   |
|                         | SCL (I²C)               | GPIO 22                  |                         |                              |                                              |
| Pulse Input (Mini jack) | Signal (PPQN)           | GPIO 35                  | —                       | GND from synth to ESP32 GND  | Use optocoupler or resistor divider          |
| Button - Yellow         | Digital input           | GPIO 17                  | —                       | GND of ESP32                 |                                              |
| Button - Red            | Digital input           | GPIO 16                  | —                       | GND of ESP32                 |                                              |
| ESP32 DevKit            | —                       | -                        | USB                     | Shared GND                   | Common GND required for all components       |

---

## 🔤 Font System

- 6x6 pixel font stored in `PROGMEM` in `FONT_6X6[]`.
- Each character is defined by 6 bytes (rows).
- Horizontal scrolling (with 1 pixel gap between letters).
- Custom drawing function handles row-by-row rendering.

---

## 🖼️ Display Modes

1. **Clock Mode**: HHMM, DDMM or YYYY display using the RTC module.
2. **Free Mode**: Display visual efects with no specific sync source.
3. **Micro Mode**: Detects loud sounds and triggers effects.
4. **Pulse Mode**: Animates in sync with incoming PPQN clock pulses. **Not yet implemented**
5. **Demo Mode**: Display random CLOCK and FREE pages every 30 seconds.
6. **Twitch Mode**: Display Twitch stream information alternatively with the selected visual effect.

---

## 🕹️ Buttons

- Red short : Start display (if off) / Change mode (normal mode) / Change page (menu mode)
- Red long : Enter the menu (normal mode) / Exit the menu (menu mode)
- Yellow short : Start display (if off) / Change page (normal mode) / Change value (menu mode) / Sync NTP (menu mode) / Launch/Quit Twitch/Demo modes (menu mode)
- Yellow long : Save option (menu mode)

---

## 🎛️ Settings Menu

- CASE TEMP : Display case temperature (from RTC)
- ESP32 TEMP : Display ESP32 temperature
- LED INTENSITY : 10 to 99%
- SCROLL SPEED : Slow (SL), Moderate (MD, default) or Fast (FS)
- FX SPEED : 10 (1s) to 99 (50ms) 
- PULSE PPQN : 1, 2, 4, 8, 12, 16, 24 (default), 32, 48, 64 or 96
- TIME FORMAT : 12 or 24 (default)
- SYNC NTP : GO (execute synchronization via WiFi)
- TWITCH MODE : GO (Launch Twitch mode)
- DEMO MODE : GO (Launch demo mode)

---

## ⚠️ Notes

- All modules must share a **common GND** for stable operation.
- Avoid using GPIOs 36/39 with Wi-Fi as they may conflict.

---

## 💡 Future Ideas

- Add Bluetooth/Wi-Fi message update
- Integration with MIDI sync
- Brightness control via ambient light
- Scrolling Text Messages
- Free visualization (without synchronization)

---

## 📸 Inspiration

Inspired by the minimalist style of the **IKEA Frekvens** speaker and light system. This project is designed to be visually simple, responsive to music, and evolutive.

---

## 🧾 License

This project is licensed under the MIT License.
Feel free to use, modify, and distribute under the terms of this license.
