/*
* File: constants.h
* Project: Darkroom Enlarger Timer
* File Created: Wednesday, 26th February 2025 8:32:50 pm
* Author: Andrei Grichine (andrei.grichine@gmail.com)
* -----
* Last Modified: Wednesday, 26th February 2025 10:23:11 pm
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

/**
* @file constants.h
* @brief Defines global constants, pin assignments, and external variable declarations
*        for the Darkroom Enlarger Timer project.
*/
#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#define ENABLE_TESTS // ENABLE_TESTS - Define this to enable unit tests

#include "DebugUtils.h" // Debug Utilities

/**
 * @namespace TimerConfig
 * @brief Contains configuration constants for timer operations.
 *        These constants define the behavior of the exposure timer.
 */
namespace TimerConfig {
    /** @brief Maximum timer delay in milliseconds (9 minutes and 59 seconds). */
    constexpr long MAX_DELAY = 599000;
    /** @brief Timer increment in milliseconds. */
    constexpr long INCREMENT = 100;
    /** @brief Delay before turning enlarger lamp on (long-press functionality) in milliseconds. */
    constexpr unsigned long TURN_ENLARGER_LAMP_ON_DELAY = 2000;
    /** @brief Timer increment duration in microseconds (calculated from INCREMENT). */
    constexpr long DURATION = INCREMENT*1000;
    /** @brief Minimum time between EEPROM writes in milliseconds (EEPROM wear reduction). */
    constexpr unsigned long EEPROM_WRITE_DELAY = 5000;
}

/**
 * @namespace LCDLayout4x20
 * @brief Configuration parameters for a 4x20 LCD display.
 *        These constants define the layout and positioning of elements on the LCD.
 */
namespace LCDLayout4x20
{
    /** @brief Number of rows in the LCD. */
    constexpr uint8_t LCD_ROWS = 4;
    /** @brief Number of columns in the LCD. */
    constexpr uint8_t LCD_COLS = 20;
    /** @brief Row number for the first line of text on the LCD. */
    constexpr uint8_t LCD_ROW_ONE = 0;
    /** @brief Row number for the second line of text on the LCD. */
    constexpr uint8_t LCD_ROW_TWO = 1;
    /** @brief Row number for the third line of text on the LCD. */
    constexpr uint8_t LCD_ROW_THREE = 2;
    /** @brief Row number for the fourth line of text on the LCD. */
    constexpr uint8_t LCD_ROW_FOUR = 3;
    /** @brief Starting column position for the leftmost big digit. */
    constexpr uint8_t LCD_OFFSET = 3;
    /** @brief Decimal point position on a 4x20 LCD. */
    constexpr uint8_t STATIC_DOT_POSITION = 7;
    /** @brief Static "SEC" text position on a 4x20 LCD. */
    constexpr uint8_t STATIC_SEC_TEXT_POSITION = 12;
    /** @brief First big digit LCD offset. */
    constexpr uint8_t FIRST_BIG_DIGIT_OFFSET = 8;
    /** @brief Second big digit LCD offset. */
    constexpr uint8_t SECOND_BIG_DIGIT_OFFSET = 4;
    /** @brief Third big digit LCD offset. */
    constexpr uint8_t THIRD_BIG_DIGIT_OFFSET = 0;
    /** @brief Row for the last delay text. */
    constexpr uint8_t LAST_DELAY_ROW = LCD_ROW_THREE;
    /** @brief Column for the last delay text. */
    constexpr uint8_t LAST_DELAY_COL = 3;
}

/** @brief Pin number for LCD RS (Register Select) pin. */
constexpr uint8_t RS_PIN = 0;
/** @brief Pin number for LCD RW (Read/Write) pin. */
constexpr uint8_t RW_PIN = 1;
/** @brief Pin number for LCD EN (Enable) pin. */
constexpr uint8_t EN_PIN = 2;
/** @brief Pin number for LCD Backlight pin. */
constexpr uint8_t BACK_PIN = 3;
/** @brief Pin number for LCD D4 data pin. */
constexpr uint8_t D4_PIN = 4;
/** @brief Pin number for LCD D5 data pin. */
constexpr uint8_t D5_PIN = 5;
/** @brief Pin number for LCD D6 data pin. */
constexpr uint8_t D6_PIN = 6;
/** @brief Pin number for LCD D7 data pin. */
constexpr uint8_t D7_PIN = 7;
/** @brief I2C address of the LCD module. */
constexpr uint8_t I2C_ADDRESS = 0x27;

/**
 * @brief EEPROM Wear Leveling Configuration
 */
/** @brief A default value we use to initialize EEPROM, should be outside of our normal range to know if we've initialized the address */
constexpr uint8_t EEPROM_INIT_VALUE = -1;
/** @brief Location of the address tracker address */
constexpr int ADDRESS_TRACKER_ADDRESS = 4;
/** @brief Start address for wear leveling (leave some space for other data) */
constexpr int EEPROM_START_ADDRESS = 10;
/** @brief Total EEPROM size in bytes (adjust for your Arduino). 1024 bytes on the ATmega328P, 512 bytes on the ATmega168 and ATmega8, 4 KB (4096 bytes) on the ATmega1280 and ATmega2560 */
constexpr int EEPROM_END_ADDRESS = 1014;
/** @brief Maximum allowed bad blocks before warning. */
constexpr int MAX_BAD_BLOCKS = 5;

/**
 * @namespace SplashScreen
 * @brief String constants for the splash screen.  Stored in flash memory to save SRAM.
 */
namespace SplashScreen {
    /** @brief First line of the splash screen text. */
    extern const char LINE_ONE_TEXT[] PROGMEM;
    /** @brief Second line of the splash screen text. */
    extern const char LINE_TWO_TEXT[] PROGMEM;
}

/** @brief Build version number. */
extern const uint8_t BUILD_VERSION PROGMEM;
/** @brief Revision number. */
extern const uint8_t REVISION_NUMBER PROGMEM;

/** @brief LCD instance (defined in constants.cpp). */
extern LiquidCrystal_I2C lcd;
/** @brief Global time (defined in constants.cpp). */
extern unsigned long globalTime;
/** @brief Current timer delay (defined in constants.cpp). */
extern long timerDelay;
/** @brief Last stored timer delay (defined in constants.cpp). */
extern long storedTimerDelay;
/** @brief Tracks the last EEPROM write time (defined in constants.cpp). */
extern unsigned long lastEEPROMWrite;
/** @brief EEPROM address to store timer delay (defined in constants.cpp). */
extern int eeAddress;
/** @brief Flag to start the exposure timer (defined in constants.cpp). */
extern volatile bool startExposure;
/** @brief Flag to turn on the enlarger lamp (defined in constants.cpp). */
extern volatile bool turnOnEnlargerLamp;
/** @brief Flag to turn on the enlarger lamp manually (defined in constants.cpp). */
extern volatile bool turnManuallyOnEnlargerLamp;
/** @brief Index to track the current EEPROM address (defined in constants.cpp). */
extern int currentEEPROMAddressIndex;
/** @brief Counter for bad EEPROM blocks (defined in constants.cpp). */
extern int badBlocksCount;
/** @brief Flag indicating an EEPROM failure (defined in constants.cpp). */
extern bool EEPROM_FAILED;

#endif // CONSTANTS_H