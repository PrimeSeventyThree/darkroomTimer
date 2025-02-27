/*
 * File: constants.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Tuesday, 18th February 2025 6:37:15 am
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Wednesday, 26th February 2025 10:28:02 pm
 * Modified By: Andrei Grichine (andrei.grichine@gmail.com>)
 * -----
 * Copyright: 2019 - 2025. Prime73 Inc.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the 'Software'), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * -----
 * HISTORY:
 */

#include "constants.h"

/*
 * This file defines global variables and initializes an LCD instance for the
 * Darkroom Enlarger Timer project. It includes variables for tracking time,
 * timer delays, EEPROM write times, and various flags for controlling the
 * enlarger lamp and timer button states. The LiquidCrystal_I2C object is
 * configured with specific pin assignments for interfacing with an LCD.
 */

/** @brief LCD instance (initialized with I2C address and pin assignments). */
LiquidCrystal_I2C lcd(I2C_ADDRESS, EN_PIN, RW_PIN, RS_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN, BACK_PIN, POSITIVE);
/** @brief Global time (initialized to 0). */
unsigned long globalTime = 0;
/** @brief Current timer delay (initialized to 0). */
long timerDelay = 0;
/** @brief Last stored timer delay (initialized to 0). */
long storedTimerDelay = 0;
/** @brief Tracks the last EEPROM write time (initialized to 0). */
unsigned long lastEEPROMWrite = 0;
/** @brief EEPROM address to store timer delay (initialized to 0). */
int eeAddress = 0;
/** @brief Flag to start the exposure timer (initialized to false). */
volatile bool startExposure = false;
/** @brief Flag to turn on the enlarger lamp (initialized to false). */
volatile bool turnOnEnlargerLamp = false;
/** @brief Flag to turn on the enlarger lamp manually (initialized to false). */
volatile bool turnManuallyOnEnlargerLamp = false;
/** @brief Index to track the current EEPROM address (initialized to 0). */
int currentEEPROMAddressIndex = 0;
/** @brief Counter for bad EEPROM blocks (initialized to 0). */
int badBlocksCount = 0;
/** @brief Flag indicating an EEPROM failure (initialized to false). */
bool EEPROM_FAILED = false;

/**
* @namespace SplashScreen
* @brief String constants for the splash screen.  Stored in flash memory to save SRAM.
*/
namespace SplashScreen {
   /** @brief First line of the splash screen text (stored in flash memory). */
   const char LINE_ONE_TEXT[] PROGMEM = "DARKROOM";
   /** @brief Second line of the splash screen text (stored in flash memory). */
   const char LINE_TWO_TEXT[] PROGMEM = "EXPOSURE TIMER";
}

/** @brief Build version number (stored in flash memory). */
const uint8_t BUILD_VERSION PROGMEM = 2.0;
/** @brief Revision number (stored in flash memory). */
const uint8_t REVISION_NUMBER PROGMEM = 73;