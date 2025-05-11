// Includes
#include <Arduino.h>
#include "settings.h"
#include "clock.h"
#include "font6x6.h"
#include "button.h"
#include "wifiNTP.h"
#include "pulse.h"
#include "display.h"
//#include "microFX1.h"
//#include "microFX2.h"
//#include "microFX3.h"
//#include "pulseFX1.h"
//#include "pulseFX2.h"
//#include "pulseFX3.h"

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
      Serial.println("Mode : CLOCK"); // ### DEBUG ###
      break;
    case MODE_MICRO:
      matrix.clear();
      //matrix.scrollText("MICRO", settings.getScrollSpeed()); // To be adapted for temporary page title display
      Serial.println("Mode : MICRO"); // ### DEBUG ###
      break;
    case MODE_PULSE:
      matrix.clear();
      String pulseText = String("PULSE ") + String(settings.getPulsePPQN()) + String(" PPQN");
      //matrix.scrollText(pulseText, settings.getScrollSpeed()); // To be adapted for temporary page title display
      Serial.println("Mode : " + pulseText); // ### DEBUG ###
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
      Serial.println("New Page"); // ### DEBUG ###
      break;
    case MODE_PULSE:
      //displayPulsePage();
      Serial.println("New Page"); // ### DEBUG ###
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
  }
  
  matrix.scrollText(optionText, settings.getScrollDelay());
  matrix.displayValue(valueText);
  Serial.println(optionText + ", Value = " + valueText); // ### DEBUG ###
}

// Buttons callbacks
void onRedButtonShortPress() {
  if (currentState == STATE_OFF) {
    currentState = STATE_RUN;
    matrix.displayValue("CIAO");
    delay(2000); 
    Serial.println("State : RUN"); // ### DEBUG ###
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
    currentState = STATE_SET;
    handleSettings();
    Serial.println("State : SET"); // ### DEBUG ###
  }
  else if(currentState == STATE_SET) {
    currentState = STATE_RUN;
    changeMode(currentMode);
    Serial.println("State : RUN"); // ### DEBUG ###
  }
}

void onYellowButtonShortPress() {
  if (currentState == STATE_OFF) {
    currentState = STATE_RUN;
    matrix.displayValue("CIAO");
    delay(2000); 
    Serial.println("State : RUN"); // ### DEBUG ###
  }
  else if(currentState == STATE_RUN) {
    changePage((currentPage + 1) % 3);
  }
  else if(currentState == STATE_SET) {
    settings.nextValue(currentOption);
    handleSettings();
  }
}

void onYellowButtonLongPress() {
  if(currentState == STATE_SET) {
    if(currentOption == OPTION_SYNC_NTP) {
      // Display sync status
      //matrix.clear();
      matrix.displayValue("SYNC");
      Serial.print("SYNC NTP : "); // ### DEBUG ###
      // Sync execution
      if(wifiNTP.sync()) {
        matrix.displayValue("  OK");
        Serial.println("OK"); // ### DEBUG ###
      } else {
        matrix.displayValue("FAIL");
        Serial.println("FAIL"); // ### DEBUG ###
      }
      delay(2000);
      handleSettings();
    }
    else {
      settings.saveSettings();
      matrix.displayValue("SAVE");
      delay(1000);
      matrix.displayValue("  OK");
      delay(1000);
      handleSettings();
      Serial.println("SAVED"); // ### DEBUG ###
    }
  }
}

void setup() {
  Serial.begin(115200); // ### DEBUG ###
  Serial.println("START!"); // ### DEBUG ###

  // Init LED matrix
  matrix.begin();
  matrix.setIntensity(settings.getLedIntensity());
  matrix.clear();
  matrix.update(); 

  // Init buttons
  redButton.begin();
  yellowButton.begin();

  // Buttons callbacks configuration
  redButton.setShortPressCallback(onRedButtonShortPress);
  redButton.setLongPressCallback(onRedButtonLongPress);
  yellowButton.setShortPressCallback(onYellowButtonShortPress);
  yellowButton.setLongPressCallback(onYellowButtonLongPress);
 
  // Init RTC Clock
  rtcClock.begin();

  // Init Pulse
  pulse.begin();

  // Init FX
  //microFX1.begin();
  //microFX2.begin();
  //microFX3.begin();
  //pulseFX1.begin();
  //pulseFX2.begin();
  //pulseFX3.begin();
  
  // Load parameters
  settings.loadSettings();
  
  // Initial state
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