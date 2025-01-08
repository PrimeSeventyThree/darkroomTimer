/*
 * File: constants.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Tuesday, 7th January 2025 11:28:30 am
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Wednesday, 8th January 2025 7:18:43 am
 * Modified By: Andrei Grichine (andrei.grichine@gmail.com>)
 * -----
 * Copyright 2019 - 2025, Prime73 Inc. MIT License
 * 
 * Copyright (c) 2025 Prime73 Inc.
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

#include "constants.h"

/*
 * This file defines global variables and initializes an LCD instance for the
 * Darkroom Enlarger Timer project. It includes variables for tracking time,
 * timer delays, EEPROM write times, and various flags for controlling the
 * enlarger lamp and timer button states. The LiquidCrystal_I2C object is
 * configured with specific pin assignments for interfacing with an LCD.
 */
unsigned long _micro = 0;     // Microseconds counter
unsigned long time = 0;        // Current time

long timerDelay = 0;           // Current timer delay
long storedTimerDelay = 0;     // Last stored timer value

unsigned long lastEEPROMWrite = 0;  // Tracks the last EEPROM write time
int eeAddress = 0;                 // EEPROM address to store timer delay

volatile bool startExposure = false; // Flag to start the exposure timer
volatile bool turnOnEnlargerLamp = false; // Flag to turn on the enlarger lamp
volatile bool turnManuallyOnEnlargerLamp = false; // Flag to turn on the enlarger lamp manually
volatile bool timerButtonIsPressed = false; // Flag to indicate the timer button is pressed

// --- LCD Instance ---
LiquidCrystal_I2C lcd(I2C_ADDRESS, EN_PIN, RW_PIN, RS_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN, BACK_PIN, POSITIVE);
