// Includes
#include <Arduino.h>
#include "settings.h"
#include "clock.h"
#include "font6x6.h"
#include "button.h"
#include "wifiNTP.h"
#include "pulse.h"
#include "display.h"
#include "debug.h"
//#include "microFX1.h"
//#include "microFX2.h"
//#include "microFX3.h"
//#include "pulseFX1.h"
//#include "pulseFX2.h"
//#include "pulseFX3.h"

// For ESP32 temperature
#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif

// Defines for pins
#define PIN_BTN_YELLOW  17 // GPIO17 for yellow button
#define PIN_BTN_RED     16 // GPIO16 for red button

// Defines for modes
#define MODE_CLOCK      0
#define MODE_MICRO      1
#define MODE_PULSE      2

// Defines for pages
#define PAGE_CLOCK_TIME 0
#define PAGE_CLOCK_DATE 1
#define PAGE_CLOCK_YEAR 2

#define PAGE_MICRO_FX1  0
#define PAGE_MICRO_FX2  1
#define PAGE_MICRO_FX3  2

#define PAGE_PULSE_FX1  0
#define PAGE_PULSE_FX2  1
#define PAGE_PULSE_FX3  2

// Stat of the system
enum SystemState {
  STATE_OFF,
  STATE_RUN,
  STATE_SET
};

// Options of the menu
enum SettingsOption {
  OPTION_LED_INTENSITY,
  OPTION_SCROLL_SPEED,
  OPTION_TIME_FORMAT,
  OPTION_SYNC_NTP,
  OPTION_PULSE_PPQN,
  OPTION_CASE_TEMP,
  OPTION_ESP32_TEMP, 
  OPTION_COUNT
};

// Global variables
SystemState    currentState  = STATE_OFF;
uint8_t        currentMode   = MODE_CLOCK;
uint8_t        currentPage   = 0;
SettingsOption currentOption = OPTION_LED_INTENSITY;

// Managment
Display  matrix;
Button   redButton(PIN_BTN_RED);
Button   yellowButton(PIN_BTN_YELLOW);
Settings settings;
Clock    rtcClock;
WifiNTP  wifiNTP(&rtcClock);
Pulse    pulse;

// Clock pages display
void displayClockPage() {
  String pageTitle;
  String pageContent;
  
  switch(currentPage) {
    case PAGE_CLOCK_TIME:
      pageTitle   = "TIME";
      pageContent = rtcClock.getTimeString(settings.getTimeFormat());
      break;
    case PAGE_CLOCK_DATE:
      pageTitle   = "DATE";
      pageContent = rtcClock.getDateString();
      break;
    case PAGE_CLOCK_YEAR:
      pageTitle   = "YEAR";
      pageContent = rtcClock.getYearString();
      break;
  }
  
  //matrix.scrollText(pageTitle, settings.getScrollSpeed()); // To be adapted for temporary page title display
  matrix.displayValue(pageContent);
}

// Change mode function
void changeMode(uint8_t newMode) {
  //Assignment
  currentMode = newMode;
  currentPage = 0;
  
  //Display
  switch(newMode) {
    case MODE_CLOCK:
      matrix.clear();
      displayClockPage();
      DEBUG_PRINTLN("Mode : CLOCK");
      break;
    case MODE_MICRO:
      matrix.clear();
      //matrix.scrollText("MICRO", settings.getScrollSpeed()); // To be adapted for temporary page title display
      DEBUG_PRINTLN("Mode : MICRO");
      break;
    case MODE_PULSE:
      matrix.clear();
      String pulseText = String("PULSE ") + String(settings.getPulsePPQN()) + String(" PPQN");
      //matrix.scrollText(pulseText, settings.getScrollSpeed()); // To be adapted for temporary page title display
      DEBUG_PRINTLN("Mode : " + pulseText);
      break;
  }
}

// Page change function
void changePage(uint8_t newPage) {
  // Assignment
  currentPage = newPage;
  
  // Display
  switch(currentMode) {
    case MODE_CLOCK:
      displayClockPage();
      break;
    case MODE_MICRO:
      //displayMicroPage();
      DEBUG_PRINTLN("New Page");
      break;
    case MODE_PULSE:
      //displayPulsePage();
      DEBUG_PRINTLN("New Page");
      break;
  }
}

// Settings menu handling
void handleSettings() {
  String optionText;
  String valueText;
  
  switch(currentOption) {
    case OPTION_LED_INTENSITY:
      optionText = "LED INTENSITY";
      valueText = String(settings.getLedIntensity());
      matrix.setIntensity(settings.getLedIntensity()); // Change directement l'intensité des LED
      break;
    case OPTION_SCROLL_SPEED:
      optionText = "SCROLL SPEED";
      valueText = settings.getScrollSpeedText();
      break;
    case OPTION_TIME_FORMAT:
      optionText = "TIME FORMAT";
      valueText = String(settings.getTimeFormat());
      break;
    case OPTION_SYNC_NTP:
      optionText = "SYNC NTP";
      valueText = "GO";
      break;
    case OPTION_PULSE_PPQN:
      optionText = "PULSE PPQN";
      valueText = String(settings.getPulsePPQN());
      break;
    case OPTION_CASE_TEMP: {
      optionText = "CASE TEMP";
      float caseTemp = rtcClock.getTemperature();
      int caseTempInt = static_cast<int>(caseTemp + 0.5f); // Round to nearest integer
      valueText = (caseTempInt < 10 ? " " : "") + String(caseTempInt); // Add leading space if single digit
      break;
    }
    case OPTION_ESP32_TEMP: {
      optionText = "ESP32 TEMP";
      float espTemp = (temprature_sens_read() - 32) / 1.8;  // Convert Fahrenheit to Celsius
      int espTempInt = static_cast<int>(espTemp + 0.5f);
      valueText = (espTempInt < 10 ? " " : "") + String(espTempInt);
      break;
    }
  }
  
  matrix.scrollText(optionText, settings.getScrollDelay());
  matrix.displayValue(valueText);
  DEBUG_PRINTLN(optionText + ", Value = " + valueText);
}

// Buttons callbacks
void onRedButtonShortPress() {
  if (currentState == STATE_OFF) {
    DEBUG_PRINTLN("State : RUN");
    currentState = STATE_RUN;
    matrix.displayValue("CIAO");
    delay(2000); 
  }
  if (currentState == STATE_RUN) {
    changeMode((currentMode + 1) % 3);
  }
  else if(currentState == STATE_SET) {
    currentOption = (SettingsOption)((currentOption + 1) % OPTION_COUNT);
    handleSettings();
  }
}

void onRedButtonLongPress() {
  if(currentState == STATE_RUN) {
    DEBUG_PRINTLN("State : SET");
    currentState = STATE_SET;
    handleSettings();
  }
  else if(currentState == STATE_SET) {
    DEBUG_PRINTLN("State : RUN");
    currentState = STATE_RUN;
    changeMode(currentMode);
  }
}

void onYellowButtonShortPress() {
  if (currentState == STATE_OFF) {
    DEBUG_PRINTLN("State : RUN");
    currentState = STATE_RUN;
    matrix.displayValue("CIAO");
    delay(2000);
  }
  else if(currentState == STATE_RUN) {
    changePage((currentPage + 1) % 3);
  }
  else if(currentState == STATE_SET) {
// For temperature options, just refresh the display
    if(currentOption == OPTION_CASE_TEMP || currentOption == OPTION_ESP32_TEMP) {
      handleSettings();
    } else {
      settings.nextValue(currentOption);
      handleSettings();
    }
  }
}

void onYellowButtonLongPress() {
  if(currentState == STATE_SET) {
    if(currentOption == OPTION_SYNC_NTP) {
      // Display sync status
      //matrix.clear();
      DEBUG_PRINT("Sync NTP : ");
      matrix.displayValue("SYNC");
      // Sync execution
      if(wifiNTP.sync()) {
        matrix.displayValue("  OK");
        DEBUG_PRINTLN("OK");
      } else {
        matrix.displayValue("FAIL");
        DEBUG_PRINTLN("FAIL");
      }
      delay(2000);
      handleSettings();
    }
    else {
      DEBUG_PRINT("SAVE : ");
      settings.saveSettings();
      matrix.displayValue("SAVE");
      delay(1000);
      matrix.displayValue("  OK");
      delay(1000);
      DEBUG_PRINTLN("OK");
      handleSettings();
      
    }
  }
}

void setup() {
  // Init debug mode
  #if DEBUG_MODE
    Serial.begin(115200);
    delay(100);  // Gives time for the serial port to initialize
    DEBUG_PRINTLN("DEBUG ON!");
  #endif

  // Init LED matrix
  DEBUG_PRINTLN("Init LED Matrix");
  matrix.begin();
  matrix.setIntensity(settings.getLedIntensity());
  matrix.clear();
  matrix.update(); 

  // Init buttons
  DEBUG_PRINTLN("Init buttons");
  redButton.begin();
  yellowButton.begin();

  // Buttons callbacks configuration
  DEBUG_PRINTLN("Buttons callbacks configuration");
  redButton.setShortPressCallback(onRedButtonShortPress);
  redButton.setLongPressCallback(onRedButtonLongPress);
  yellowButton.setShortPressCallback(onYellowButtonShortPress);
  yellowButton.setLongPressCallback(onYellowButtonLongPress);
 
  // Init RTC Clock
  DEBUG_PRINTLN("Init RTC Clock");
  rtcClock.begin();

  // Init Pulse
  DEBUG_PRINTLN("Init Pulse");
  pulse.begin();

  // Init FX
  DEBUG_PRINTLN("Init FX Pages");
  //microFX1.begin();
  //microFX2.begin();
  //microFX3.begin();
  //pulseFX1.begin();
  //pulseFX2.begin();
  //pulseFX3.begin();
  
  // Load parameters
  DEBUG_PRINTLN("Load Parameters");
  settings.loadSettings();
  
  // Initial state
  DEBUG_PRINTLN("Current State = OFF");
  currentState = STATE_OFF;
}

// Loop
void loop() {
  // Update buttons
  redButton.update();
  yellowButton.update();

  // Update display (regarding selected mode)
  if(currentState == STATE_RUN) {
    switch(currentMode) {
      case MODE_CLOCK:
        displayClockPage();
        break;
      case MODE_MICRO:
        //displayMicroPage();
        matrix.displayValue("MICR");
        break;
      case MODE_PULSE:
        //displayPulsePage();
        matrix.displayValue("PULS");
        break;
    }
  } else {
    matrix.updateScroll();
  }

  // Delay to avoid overloading the processor
  delay(10);
}