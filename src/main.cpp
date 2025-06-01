// Includes
#include <Arduino.h>
#include "defines.h"
#include "settings.h"
#include "font6x6.h"
#include "button.h"
#include "wifiNTP.h"
#include "clock.h"
#include "display.h"

// Free FX
//#include "free.h"
#include "freeFX1.h"
#include "freeFX2.h"
#include "freeFX3.h"

// Micro FX
//#include "micro.h"
#include "microFX1.h"
#include "microFX2.h"
#include "microFX3.h"

// Pulse FX
//#include "pulse.h"
//#include "pulseFX1.h" --> To be implemented
//#include "pulseFX2.h" --> To be implemented
//#include "pulseFX3.h" --> To be implemented

// Twitch API
#include "twitchAPI.h"

// State of the system
enum SystemState {
  STATE_OFF,
  STATE_RUN,
  STATE_SET
};

// Global variables
SystemState    currentState  = STATE_RUN;        // Click any button to start if set to STATE_OFF
uint8_t        currentMode   = MODE_CLOCK;       // MODE_CLOCK by default
uint8_t        currentPage   = 0;                // PAGE_CLOCK_TIME by default
SettingsOption currentOption = OPTION_CASE_TEMP; // Default option

// Title display variables 
unsigned long lastTitleDisplay = 0;
bool displayingTitle = false;
const unsigned long TITLE_DURATION = 1000; // 1 second

// Twitch variables
unsigned long lastTwitchDisplayTime = 0;
bool displayingTwitchInfo = false;
uint8_t scrollCount = 0;
const uint8_t SCROLL_MAX = 1;

// Demo variables
bool demoMode = false;
unsigned long lastDemoChange = 0;
const unsigned long DEMO_INTERVAL = 30000; // 30 seconds
uint8_t lastDemoMode = 0;
uint8_t lastDemoPage = 0;

// Management
Display   matrix;
Button    redButton(PIN_BTN_RED);
Button    yellowButton(PIN_BTN_YELLOW);
Settings  settings;
Clock     rtcClock;
WifiNTP   wifiNTP(&rtcClock);
//Free      free;
//Micro     micro;
//Pulse     pulse;
TwitchAPI twitch;

// Visual effects
FreeFX1 freeFX1(&settings);
FreeFX2 freeFX2(&settings);
FreeFX3 freeFX3(&settings);
MicroFX1 microFX1;
MicroFX2 microFX2;
MicroFX3 microFX3;
//PulseFX1 pulseFX1(&pulse); --> To be implemented
//PulseFX2 pulseFX2(&pulse); --> To be implemented
//PulseFX3 pulseFX3(&pulse); --> To be implemented

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
  
  unsigned long currentTime = millis();

  // Show title when page changes
  if (displayingTitle) {
    if (currentTime - lastTitleDisplay >= TITLE_DURATION) {
      displayingTitle = false;
      matrix.displayValue(pageContent);
    } else {
      matrix.displayValue(pageTitle);
    }
  } else {
    matrix.displayValue(pageContent);
  }
}

//Free FX pages display
void displayFreePage() {
  unsigned long delay = settings.getFxDelay();
  
  switch(currentPage) {
    case PAGE_FREE_FX1:
      freeFX1.update(matrix);
      //matrix.displayValue("FRE1"); // TEMP
      break;
    case PAGE_FREE_FX2:
      freeFX2.update(matrix);
      //matrix.displayValue("FRE2"); // TEMP
      break;
    case PAGE_FREE_FX3:
      freeFX3.update(matrix);
      //matrix.displayValue("FRE3"); // TEMP
      break;
  }
}

// Micro pages display
void displayMicroPage() {
  switch(currentPage) {
    case PAGE_MICRO_FX1:
      microFX1.update(matrix);
      //matrix.displayValue("MIC1"); // TEMP
      break;
    case PAGE_MICRO_FX2:
      microFX2.update(matrix);
      //matrix.displayValue("MIC2"); // TEMP
      break;
    case PAGE_MICRO_FX3:
      microFX3.update(matrix);
      //matrix.displayValue("MIC3"); // TEMP
      break;
  }
}

// Pulse pages display
void displayPulsePage() {
  switch(currentPage) {
    case PAGE_PULSE_FX1:
      //pulseFX1.update(matrix);
      matrix.displayValue("PLS1"); // TEMP
      break;
    case PAGE_PULSE_FX2:
      //pulseFX2.update(matrix);
      matrix.displayValue("PLS2"); // TEMP
      break;
    case PAGE_PULSE_FX3:
      //pulseFX3.update(matrix);
      matrix.displayValue("PLS3"); // TEMP
      break;
  }
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
      lastTitleDisplay = millis();
      displayingTitle = true;      
      displayClockPage();
      DEBUG_PRINTLN("  Mode : CLOCK");
      break;
    case MODE_FREE:
      matrix.clear();
      //matrix.scrollText("FREE", settings.getFxDelay()); // To be adapted for temporary page title display
      DEBUG_PRINTLN("  Mode : FREE");
      break;      
    case MODE_MICRO:
      matrix.clear();
      //matrix.scrollText("MICRO", settings.getScrollDelay()); // To be adapted for temporary page title display
      DEBUG_PRINTLN("  Mode : MICRO");
      break;
    case MODE_PULSE:
      matrix.clear();
      String pulseText = String("PULSE (") + String(settings.getPulsePPQN()) + String(" PPQN)");
      //matrix.scrollText(pulseText, settings.getScrollDelay()); // To be adapted for temporary page title display
      DEBUG_PRINTLN("  Mode : " + pulseText);
      break;
  }
  DEBUG_PRINTLN("    Page : " + String(currentPage + 1));
}

// Page change function
void changePage(uint8_t newPage) {
  // Assignment
  currentPage = newPage;

  // Reset FreeFX3 when switching to it
  if (currentMode == MODE_FREE && newPage == PAGE_FREE_FX3) {
      freeFX3.begin();
  }

  // Display
  switch(currentMode) {
    case MODE_CLOCK:
      lastTitleDisplay = millis();
      displayingTitle = true;
      displayClockPage();
      break;
    case MODE_FREE:
      displayFreePage();
      break;
    case MODE_MICRO:
      displayMicroPage();
      break;
    case MODE_PULSE:
      displayPulsePage();
      break;
  }
}

// Settings menu handling
void handleSettings() {
  String optionText;
  String valueText;
  
  switch(currentOption) {
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
    case OPTION_LED_INTENSITY:
      optionText = "LED INTENSITY";
      valueText = String(settings.getLedIntensity());
      matrix.setIntensity(settings.getLedIntensity()); // Change instantly LED intensity
      break;
    case OPTION_SCROLL_SPEED:
      optionText = "SCROLL SPEED";
      valueText = settings.getScrollSpeedText();
      break;
    case OPTION_FX_SPEED:
      optionText = "FX SPEED";
      valueText = String(settings.getFxSpeed());
      break; 
    case OPTION_PULSE_PPQN:
      optionText = "PULSE PPQN";
      valueText = String(settings.getPulsePPQN());
      break;
    case OPTION_TIME_FORMAT:
      optionText = "TIME FORMAT";
      valueText = String(settings.getTimeFormat());
      break;
    case OPTION_SYNC_NTP:
      optionText = "SYNC NTP";
      valueText = "GO";
      break;
    case OPTION_TWITCH_MODE:
      optionText = "TWITCH MODE";
      valueText = "GO";
      break;
    case OPTION_DEMO_MODE:
      optionText = "DEMO MODE";
      valueText = "GO";
      break;  
  }
  
  matrix.scrollText(optionText, settings.getScrollDelay());
  matrix.displayValue(valueText);
  DEBUG_PRINTLN("  " + optionText + " : " + valueText);
}

// Buttons callbacks
void onRedButtonShortPress() {
  if (currentState == STATE_OFF) {
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("State : RUN");
    currentState = STATE_RUN;
    matrix.displayValue("CIAO");
    delay(2000); 
  }
  if (currentState == STATE_RUN) {
    if (demoMode) {
      demoMode = false;
      DEBUG_PRINTLN("  Demo Mode : OFF");
      DEBUG_PRINTLN();
      //currentMode = MODE_CLOCK;
      //currentPage = 0;
      //matrix.displayPage(currentMode, currentPage);
    }
    else if (twitch.isEnabled()) {
      matrix.stopScroll();
      twitch.disable();
      DEBUG_PRINTLN("  Twitch Mode : OFF"); 
      DEBUG_PRINTLN(); 
    } else {   
      changeMode((currentMode + 1) % 3);
    }

  }
  else if (currentState == STATE_SET) {
    currentOption = (SettingsOption)((currentOption + 1) % OPTION_COUNT);
    handleSettings();
  }
}

void onRedButtonLongPress() {
  if (currentState == STATE_RUN) {
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("State : SET");
    currentState = STATE_SET;
    handleSettings();
  }
  else if (currentState == STATE_SET) {
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("State : RUN");
    currentState = STATE_RUN;
    matrix.stopScroll();
    // Do not reset current page
    switch(currentMode) {
      case MODE_CLOCK:
        displayClockPage();
        break;
      case MODE_FREE:
        displayFreePage();
        break;
      case MODE_MICRO:
        displayMicroPage();
        break;
      case MODE_PULSE:
        displayPulsePage();
        break;
    }
  }
}

void onYellowButtonShortPress() {
  if (currentState == STATE_OFF) {
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("State : RUN");
    currentState = STATE_RUN;
    matrix.displayValue("CIAO");
    delay(2000);
  }
  if (currentState == STATE_RUN) {
    if (demoMode) {
      demoMode = false;
      DEBUG_PRINTLN("  Demo Mode : OFF");
      DEBUG_PRINTLN();
      //currentMode = MODE_CLOCK;
      //currentPage = 0;
      //matrix.displayPage(currentMode, currentPage);
    }
    else if (twitch.isEnabled()) {
      matrix.stopScroll(); 
      twitch.disable();
      DEBUG_PRINTLN("  Twitch Mode : OFF");
      DEBUG_PRINTLN();  
    } else {
      changePage((currentPage + 1) % 3);
      DEBUG_PRINTLN("    Page : " + String(currentPage + 1));
    }
  }
  else if (currentState == STATE_SET) {
    if (currentOption == OPTION_CASE_TEMP || currentOption == OPTION_ESP32_TEMP) {
      handleSettings();  // Just refresh temperatures
    } 
    else if (currentOption == OPTION_SYNC_NTP) {
      // Display sync status
      matrix.displayValue("SYNC");
      if (wifiNTP.sync()) {
        matrix.displayValue("  OK");
        DEBUG_PRINTLN("  SYNC NTP : SUCCESS");
      } else {
        matrix.displayValue("FAIL");
        DEBUG_PRINTLN("  SYNC NTP : FAIL");
      }
      DEBUG_PRINTLN();
      delay(2000);
      handleSettings();
    }
    else if (currentOption == OPTION_TWITCH_MODE) {
      //matrix.stopScroll();
      matrix.displayValue("SYNC");
      lastTwitchDisplayTime = 0;
      displayingTwitchInfo = false;
      twitch.enable();
      currentState = STATE_RUN;
    }
    else if (currentOption == OPTION_DEMO_MODE) {
      matrix.stopScroll(); 
      matrix.displayValue("DEMO");
      delay(1000);
      demoMode = true;
      lastDemoChange = 0;
      // Init with random mode/page
      currentMode = random(3);  // 0 to 2
      currentPage = random(3);  // 0 to 2
      currentState = STATE_RUN;
    }
    else {
      settings.nextValue(currentOption);  // Change value for all other options
      handleSettings();
    }
  }
}

void onYellowButtonLongPress() {
  if (currentState == STATE_SET) {
    DEBUG_PRINT("> SAVE PARAM : ");
    settings.saveSettings();
    matrix.displayValue("SAVE");
    delay(1000);
    matrix.displayValue("  OK");
    delay(1000);
    DEBUG_PRINTLN("OK");
    handleSettings();
  }
}

void setup() {
  // Init debug mode
  #if DEBUG_MODE
    Serial.begin(115200);
    delay(1000);  // Gives time for the serial port to initialize
    DEBUG_PRINTLN("DEBUG ON!");
  #endif

  DEBUG_PRINTLN();
  DEBUG_PRINTLN("SETUP");

  // Init LED matrix
  DEBUG_PRINTLN("  Init LED Matrix");
  matrix.begin();
  matrix.setIntensity(settings.getLedIntensity());
  matrix.clear();
  matrix.update(); 

  // Init buttons
  DEBUG_PRINTLN("  Init buttons");
  redButton.begin();
  yellowButton.begin();

  // Buttons callbacks configuration
  DEBUG_PRINTLN("  Buttons callbacks configuration");
  redButton.setShortPressCallback(onRedButtonShortPress);
  redButton.setLongPressCallback(onRedButtonLongPress);
  yellowButton.setShortPressCallback(onYellowButtonShortPress);
  yellowButton.setLongPressCallback(onYellowButtonLongPress);
 
  // Init RTC Clock
  DEBUG_PRINTLN("  Init RTC Clock");
  rtcClock.begin();

  // Init Free
  DEBUG_PRINTLN("  Init Free --> To be verified");
  //free.begin(); --> To be verified

  // Init Pulse
  DEBUG_PRINTLN("  Init Pulse --> To be verified");
  //pulse.begin(); --> To be implemented

  // Init FX
  DEBUG_PRINTLN("  Init FX Pages");
  freeFX1.begin();
  freeFX2.begin();
  freeFX3.begin();
  microFX1.begin();
  microFX2.begin();
  microFX3.begin();
  //pulseFX1.begin(); --> To be implemented
  //pulseFX2.begin(); --> To be implemented
  //pulseFX3.begin(); --> To be implemented
  
  // Init Twitch Mode
  DEBUG_PRINTLN("  Init Twitch");
  twitch.begin();

  // Load parameters
  DEBUG_PRINTLN("  Load Parameters");
  settings.loadSettings();
  
  // Initial state
  DEBUG_PRINTLN("  Default State = OFF");
  currentState = STATE_OFF;
  DEBUG_PRINTLN("  Change current State = RUN");
  currentState = STATE_RUN;
  matrix.displayValue("CIAO");
  delay(2000);
}

void loop() {
  // Update buttons
  redButton.update();
  yellowButton.update();

  // Update display (regarding selected mode)
  if(currentState == STATE_RUN) {
    if (demoMode) {
      unsigned long currentTime = millis();
      if (currentTime - lastDemoChange >= DEMO_INTERVAL) {
        lastDemoChange = currentTime;
        
        // Choose new random mode and page, avoiding last combination
        uint8_t newMode, newPage;
        do {
            newMode = random(2); // Only CLOCK and FREE modes
            newPage = random(3);
        } while (newMode == lastDemoMode && newPage == lastDemoPage);
        
        DEBUG_PRINTLN();
        DEBUG_PRINTLN("DEMO MODE CHANGE");
        DEBUG_PRINTLN("  Previous Mode/Page : " + String(lastDemoMode) + "/" + String(lastDemoPage));
        DEBUG_PRINTLN("  New Mode/Page : " + String(newMode) + "/" + String(newPage));
        
        // Save current as last before changing
        lastDemoMode = currentMode;
        lastDemoPage = currentPage;
        
        // Update current mode and page
        currentMode = newMode;
        currentPage = newPage;
        matrix.clear();
      } else {  
        switch(currentMode) {
          case MODE_CLOCK:
            displayClockPage();
            break;
          case MODE_FREE:
            displayFreePage();
            break;
          case MODE_MICRO:
            displayMicroPage();
            break;
          case MODE_PULSE:
            displayPulsePage();
            break;
        }
      }
    }
    // Handle Twitch display updates
    else if (twitch.isEnabled()) {
      unsigned long currentTime = millis();
      
      // Handle display window every minute
      if (!displayingTwitchInfo && (currentTime - lastTwitchDisplayTime >= 60000 || lastTwitchDisplayTime == 0)) {
        DEBUG_PRINTLN();
        DEBUG_PRINTLN("  TWITCH SCROLL : START");
        displayingTwitchInfo = true;
        lastTwitchDisplayTime = currentTime;

        String displayText = twitch.getNextStatusText(STATUS_CYCLE); // STATUS_CYCLE (default) or STATUS_RANDOM
        DEBUG_PRINTLN("    Display Text : " + displayText);
        matrix.clear();
        matrix.scrollText(displayText, settings.getScrollDelay(), 1); // Scroll one time
      } 
      
      // Check if scroll is complete
      if (displayingTwitchInfo && matrix.isScrollComplete()) {
        DEBUG_PRINTLN("  TWITCH SCROLL STOP");
        displayingTwitchInfo = false;
        matrix.stopScroll();
      }
      
      if (!displayingTwitchInfo) {
        // Return to normal display
        switch(currentMode) {
          case MODE_CLOCK:
            displayClockPage();
            break;
          case MODE_FREE:
            displayFreePage();
            break;
          case MODE_MICRO:
            displayMicroPage();
            break;
          case MODE_PULSE:
            displayPulsePage();
            break;
        }
      }
    }
    else {
      // Normal mode display
      switch(currentMode) {
        case MODE_CLOCK:
          displayClockPage();
          break;
        case MODE_FREE:
          displayFreePage();
          break;
        case MODE_MICRO:
          displayMicroPage();
          break;
        case MODE_PULSE:
          displayPulsePage();
          break;
      }
    }
  }
  
  matrix.updateScroll();
  delay(10);
}