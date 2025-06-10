// Includes
#include <Arduino.h>
#include "settings.h"

// For ESP32 temperature
#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif

// Defines for modes
#define MODE_CLOCK      0
#define MODE_FREE       1
#define MODE_MICRO      2
#define MODE_PULSE      3

// Pages per mode
#define CLOCK_PAGES 3  // TIME, DATE, YEAR
#define FREE_PAGES  4  // FX1, FX2, FX3, FX4
#define MICRO_PAGES 4  // FX1, FX2, FX3, FX4
#define PULSE_PAGES 3  // FX1, FX2, FX3

// Get number of pages for a given mode
#define GET_MODE_PAGES(mode) \
    ((mode) == MODE_CLOCK ? CLOCK_PAGES : \
     (mode) == MODE_FREE  ? FREE_PAGES  : \
     (mode) == MODE_MICRO ? MICRO_PAGES : \
     (mode) == MODE_PULSE ? PULSE_PAGES : 0)

// Defines for pages
#define PAGE_CLOCK_TIME 0
#define PAGE_CLOCK_DATE 1
#define PAGE_CLOCK_YEAR 2

#define PAGE_FREE_FX1  0
#define PAGE_FREE_FX2  1
#define PAGE_FREE_FX3  2
#define PAGE_FREE_FX4  3

#define PAGE_MICRO_FX1  0
#define PAGE_MICRO_FX2  1
#define PAGE_MICRO_FX3  2
#define PAGE_MICRO_FX4  3

#define PAGE_PULSE_FX1  0
#define PAGE_PULSE_FX2  1
#define PAGE_PULSE_FX3  2

// Defines for debug mode
#define DEBUG_MODE 1 // set to 0 to disable logs

#if DEBUG_MODE
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif