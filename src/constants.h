/*
 * File: constants.h
 * Project: Darkroom Enlarger Timer
 * File Created: Tuesday, 31st December 2024 3:09:25 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Tuesday, 7th January 2025 12:41:48 pm
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
#include <LiquidCrystal_I2C.h>
#include "DebugUtils.h" // Debug Utils

#define build 1
#define revision 6

extern unsigned long _micro;
extern unsigned long time;

extern LiquidCrystal_I2C lcd;

// Timer Configuration
#define MAX_TIMER_DELAY 599000        // 9 minutes and 59 seconds in milliseconds
#define TIMER_INCREMENT 100           // // change this value to change the milliseconds increment when setting the timer delay
#define TURN_ENLARGER_LAMP_ON_DELAY 2000 // Delay before turning the enlarger lamp ON
const unsigned long duration = 100000;        // timer increment in microseconds (100000us = 100ms)
// Button Debounce Configuration
#define DEBOUNCE_DELAY 50             // Debounce time in milliseconds

// LCD Display Configuration
#define LCD_ROWS 4                    // Number of rows in the LCD
#define LCD_COLS 20                   // Number of columns in the LCD
#define I2C_ADDRESS 0x27              // I2C address of the LCD module
#define RS_pin 0
#define RW_pin 1
#define EN_pin 2
#define BACK_pin 3
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7


// Relay Configuration
#define RELAY_PIN 7                   // Relay to control the enlarger lamp
#define MANUAL_LIGHT_PIN 8            // Indicator for manual light mode

// Timer Button Configuration
#define TIMER_BUTTON_PIN 6            // Timer start button
// VARIABLES:
// Rotary's encoder middle pin should be connected to a ground
// flip left and right pins to change rotation directions to modify the timer delay

#define LCD_OFFSET 3      // sets the display position for the very left BIG digit (we use only three digits for now)
const int timerButtonPin = 6; // timer start push button connection pin
const int relayOnePin = 7;    //  the pin number of the relay controlling the enlarger lamp
const int manualLightPin = 8; // manual light indicator pin

// Global Variables
extern volatile bool startExposure; // Flag to start the exposure timer
extern long timerDelay;           // Current timer delay
extern long storedTimerDelay;     // Last stored timer value
extern unsigned long lastEEPROMWrite;  // Tracks the last EEPROM write time


// Rotary's encoder middle pin should be connected to a ground
// flip left and right pins to change rotation directions to modify the timer delay
const int rotaryEncoderPinOne = 3; // left pin (A)
const int rotaryEncoderPinTwo = 2; // right pin (B)

const int rotaryEncoderSpeedLimit = 2; // the rotary encoder speed threshold above which the increment value is multiplied by the current speed value. This is to avoid the encoder to be too sensitive when setting the timer delay.
#define ROTARY_ENCODER_BUTTON_PIN 4        // rotary encoder's push button connection pin (resets timer to 0)

extern volatile bool startExposure;
extern volatile bool turnOnEnlargerLamp;         // A flag indicating whether the enlarger lamp should be turned on or off. Stores an enlarger's lamp state in timer mode (Enlarger is turned on/off automatically).
extern volatile bool turnManuallyOnEnlargerLamp; // A flag indicating whether the enlarger lamp is manually turned on or off. Stores an enlarger's lamp state in manual mode (Enlarger is turned on/off manually).
extern volatile bool timerButtonIsPressed;       // stores a timer button state (pressed or not pressed)

extern int eeAddress; // A EEPROM memory location to store the exposure time between power cycles. This is used to restore the exposure time after the power cycle.

#endif // CONSTANTS_H

