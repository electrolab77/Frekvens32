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
#include "freeFX4.h"

// Micro FX
//#include "micro.h"
#include "microFX1.h"
#include "microFX2.h"
#include "microFX3.h"
#include "microFX4.h"

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

// Blink duration
static const unsigned long BLINK_SLOW = 500;  // 500ms per state
static const unsigned long BLINK_FAST = 250;  // 250ms per state

// Twitch variables
unsigned long lastTwitchDisplayTime = 0;
bool displayingTwitchInfo = false;
uint8_t scrollCount = 0;
const uint8_t SCROLL_MAX = 1;
uint8_t lastTwitchMode = MODE_CLOCK;
uint8_t lastTwitchPage = 0;

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
FreeFX4 freeFX4(&settings);
MicroFX1 microFX1;
MicroFX2 microFX2;
MicroFX3 microFX3;
MicroFX4 microFX4;
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
    case PAGE_FREE_FX4:
      freeFX4.update(matrix);
      //matrix.displayValue("FRE4"); // TEMP
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
    case PAGE_MICRO_FX4:
      microFX4.update(matrix);
      //matrix.displayValue("MIC4"); // TEMP
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
  
  // Get maximum pages for current mode
  uint8_t maxPages = GET_MODE_PAGES(currentMode);

  //Display
  switch(newMode) {
    case MODE_CLOCK:
      matrix.clear();
      lastTitleDisplay = millis();
      displayingTitle = true;      
      DEBUG_PRINTLN("  Mode : CLOCK");
      displayClockPage();
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
  DEBUG_PRINTLN("    Page : " + String(currentPage + 1) + "/" + String(maxPages));
}

// Page change function
void changePage(uint8_t newPage) {
    // Get maximum pages for current mode
    uint8_t maxPages = GET_MODE_PAGES(currentMode);
    
    // Ensure page is within bounds
    currentPage = newPage % maxPages;
    
    // Reset FreeFX3 when switching to it
    if (currentMode == MODE_FREE && currentPage == PAGE_FREE_FX3) {
        freeFX3.begin();
    }
    
    DEBUG_PRINTLN("    Page : " + String(currentPage + 1) + "/" + String(maxPages));
    
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
      uint8_t maxPages = GET_MODE_PAGES(currentMode);
      changePage((currentPage + 1) % maxPages);
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
      delay(1000); // Wait 1 second
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
      DEBUG_PRINTLN();
      DEBUG_PRINTLN("DEMO MODE");
      matrix.displayValue("DEMO");
      matrix.blinkDisplay(BLINK_SLOW, 3); // Blink slowly 3 times
      //delay(1000);
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
  freeFX4.begin();
  microFX1.begin();
  microFX2.begin();
  microFX3.begin();
  microFX4.begin();
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

  DEBUG_PRINTLN("  Diplay CIAO");
  matrix.displayValue("CIAO");
  matrix.blinkDisplay(BLINK_SLOW, 3); // Blink slowly 3 times
  //delay(1000); // wait 1 second
}

void loop() {
  // Update buttons
  redButton.update();
  yellowButton.update();

  // Update display (regarding selected mode)
  if (!matrix.isBlinkActive()) {
    if(currentState == STATE_RUN) {
      if (demoMode) {
        unsigned long currentTime = millis();
        if (currentTime - lastDemoChange >= DEMO_INTERVAL) {
          lastDemoChange = currentTime;
          
          // Choose new random mode and page, avoiding last combination
          uint8_t newMode, newPage;
          do {
              newMode = random(2);  // Only 2 modes : 0=CLOCK or 1=FREE
              newPage = random(GET_MODE_PAGES(newMode));
          } while (newMode == lastDemoMode && newPage == lastDemoPage);

          DEBUG_PRINT("  DEMO MODE CHANGE : " + String(currentMode) + "/" + String(currentPage));
          DEBUG_PRINTLN(" -> " + String(newMode) + "/" + String(newPage));

          // Save current as last before changing
          lastDemoMode = currentMode;
          lastDemoPage = currentPage;
          
          // Update current mode and page
          currentMode = newMode;
          currentPage = newPage;


          //matrix.clear(); // TEMP
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
          DEBUG_PRINTLN("  TWITCH SCROLL START");
          displayingTwitchInfo = true;
          lastTwitchDisplayTime = currentTime;

          String displayText = twitch.getNextStatusText(STATUS_CYCLE);
          DEBUG_PRINTLN("    Display Text : " + displayText);
          matrix.clear();
          matrix.scrollText(displayText, settings.getScrollDelay(), 1);

          // Choose new random mode and page, avoiding last combination
          uint8_t newMode, newPage;
          do {
              newMode = random(2) + 1; // Only 2 modes : 1=FREE or 2=MICRO
              newPage = random(3);     // 0-2
          } while (newMode == lastTwitchMode && newPage == lastTwitchPage);
          
          // Save current as last before changing
          lastTwitchMode = currentMode;
          lastTwitchPage = currentPage;
          
          // Update current mode and page
          currentMode = newMode;
          currentPage = newPage;

          DEBUG_PRINT("    Set new Mode/Page : "+ String(newMode) + "/" + String(newPage));
          DEBUG_PRINTLN(" (Previous : " + String(lastTwitchMode) + "/" + String(lastTwitchPage) + ")");
        }

        // Check if scroll is complete
        if (displayingTwitchInfo && matrix.isScrollComplete()) {
          DEBUG_PRINTLN("  TWITCH SCROLL STOP");
          displayingTwitchInfo = false;
          matrix.stopScroll();
        }
        
        if (!displayingTwitchInfo) {
          // Display current random effect
          switch(currentMode) {
            case MODE_FREE:
              displayFreePage();
              break;
            case MODE_MICRO:
              displayMicroPage();
              break;
          }
        }
      } else {
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
  }

  /* --> Generate Freeze
  if (twitch.isEnabled()) { 
    if (twitch.update(matrix.isScrolling())) { 
        // Check for new events
        if (twitch.hasNewEvent()) {
            String eventMsg = twitch.getEventMessage();
            if (!eventMsg.isEmpty()) {
                matrix.scrollText(eventMsg, settings.getScrollDelay(), 1);
            }
        }
    }
  }
  */
  
  matrix.updateScroll();
  matrix.updateBlink();
  delay(10);
}