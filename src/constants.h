/*
 * File: constants.h
 * Project: Darkroom Enlarger Timer
 * File Created: Tuesday, 31st December 2024 3:09:25 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Monday, 17th February 2025 11:21:05 pm
 * Modified By: Andrei Grichine (andrei.grichine@gmail.com>)
 * -----
 * Copyright 2019 - 2024, Prime73 Inc. MIT License
 * 
 * Copyright (c) 2024 Prime73 Inc.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * -----
 * HISTORY:
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#define DEBUG           // Comment out this line to disable all debug output before uploading final sketch to a board

#include "DebugUtils.h" // Debug Utilities

/**
 * @namespace TimerConfig
 * 
 * @brief Contains configuration constants for timer operations.
 * 
 * This namespace defines constants used for configuring timer delays and increments.
 * - `MAX_DELAY`: Maximum allowable delay for the timer, set to 9 minutes and 59 seconds in milliseconds.
 * - `INCREMENT`: The increment value for the timer in milliseconds.
 * - `TURN_ENLARGER_LAMP_ON_DELAY`: Delay before turning the enlarger lamp on, specified in milliseconds.
 * - `DURATION`: Timer increment duration in microseconds, calculated as `INCREMENT` multiplied by 1000.
 * 
 * Note: The `EEPROM_WRITE_DELAY` is commented out and represents the minimum time between EEPROM writes in milliseconds.
 */
namespace TimerConfig {
    constexpr long MAX_DELAY = 599000;            // Max timer delay (9m59s in ms)
    constexpr long INCREMENT = 100;              // Timer increment in ms
    constexpr unsigned long TURN_ENLARGER_LAMP_ON_DELAY = 2000; // Delay before turning enlarger lamp on
    constexpr long DURATION = INCREMENT*1000; // Timer increment in microseconds
    //constexpr unsigned long EEPROM_WRITE_DELAY = 5000; // Min time between EEPROM writes in ms
}

/**
 * LCD Display Configuration
 *
 * This section defines the configuration parameters for an LCD display
 * using an I2C interface. It specifies the number of rows and columns
 * of the display, the I2C address, and the starting column position
 * for displaying large digits. Additionally, it defines the pin
 * assignments for the LCD control and data lines.
 *
 * Constants:
 * - LCD_ROWS: Number of rows in the LCD.
 * - LCD_COLS: Number of columns in the LCD.
 * - I2C_ADDRESS: I2C address of the LCD module.
 * - LCD_OFFSET: Starting column position for the leftmost big digit.
 * - RS_PIN, RW_PIN, EN_PIN, BACK_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN:
 *   Pin assignments for the LCD control and data lines.
 */
constexpr uint8_t LCD_ROWS = 4;                    // Number of rows in the LCD
constexpr uint8_t LCD_COLS = 20;                   // Number of columns in the LCD
constexpr uint8_t LCD_ROW_ONE = 0;
constexpr uint8_t LCD_ROW_TWO = 1;
constexpr uint8_t LCD_ROW_THREE = 2;
constexpr uint8_t LCD_ROW_FOUR = 3;
constexpr uint8_t I2C_ADDRESS = 0x27;              // I2C address of the LCD module
constexpr uint8_t LCD_OFFSET = 3;                  // Starting column position for the leftmost big digit
constexpr uint8_t STATIC_DOT_POSITION = 7;         // Decimal point position on a 4x20 LCD
constexpr uint8_t STATIC_SEC_TEXT_POSITION = 12;   // Static "SEC" text position on a 4x20 LCD
constexpr uint8_t FIRST_BIG_DIGIT_OFFSET = 8;      // First big digit LCD offset
constexpr uint8_t SECOND_BIG_DIGIT_OFFSET = 4;     // Second big digit LCD offset
constexpr uint8_t THIRD_BIG_DIGIT_OFFSET = 0;      // Third big digit LCD offset

constexpr uint8_t RS_PIN = 0;
constexpr uint8_t RW_PIN = 1;
constexpr uint8_t EN_PIN = 2;
constexpr uint8_t BACK_PIN = 3;
constexpr uint8_t D4_PIN = 4;
constexpr uint8_t D5_PIN = 5;
constexpr uint8_t D6_PIN = 6;
constexpr uint8_t D7_PIN = 7;

// --- LCD Instance ---
extern LiquidCrystal_I2C lcd; 

// --- Global Variables ---
extern unsigned long _micro;
extern unsigned long time;

extern volatile bool startExposure;
extern volatile bool turnOnEnlargerLamp;         // Flag for automatic lamp control
extern volatile bool turnManuallyOnEnlargerLamp; // Flag for manual lamp control

extern long timerDelay;           // Current timer delay
extern long storedTimerDelay;     // Last stored timer value
extern unsigned long lastEEPROMWrite;  // Tracks the last EEPROM write time
extern int eeAddress;             // EEPROM address to store timer delay

#endif // CONSTANTS_H

// --- SPLASH SCREEN TEXT ---
namespace SplashScreen {
    const char LINE_ONE_TEXT[] PROGMEM = "DARKROOM";
    const char LINE_TWO_TEXT[] PROGMEM = "EXPOSURE TIMER";
}
// --- Build Information ---
const uint8_t BUILD_VERSION PROGMEM =2.0;
const uint8_t REVISION_NUMBER PROGMEM =45;
