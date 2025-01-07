/*
 * File: darkroom_timer.ino
 * Project: Darkroom Enlarger Timer
 * File Created: Wednesday, 21st July 2021 10:40:30 am
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Tuesday, 7th January 2025 4:42:17 pm
 * Modified By: Andrei Grichine (andrei.grichine@gmail.com>)
 * -----
 * Copyright 2019 - 2022, Prime73 Inc. MIT License
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

//********************************************************************************
//
//    DARKROOM EXPOSURE TIMER
//    Andrei Grichine, December 2017
//
//    this code is based on:
//    BIG FONT (4-line) LCD CHARACTERS
//    http://woodsgood.ca/projects/2015/02/27/4-line-lcd-big-numbers/
//    Adrian Jones, February 2015
//
//    REQUIREMENTS:
//    MD_REncoder - Rotary Encoder Library (https://github.com/MajicDesigns/MD_REncoder)
//    New-LiquidCrystal - LCD Library for Arduino and Chipkit (https://github.com/fmalpartida/New-LiquidCrystal)
//********************************************************************************

//********************************************************************************

#define DEBUG           // Comment out this line to disable all debug output before uploading final sketch to a board

#include "src/DebugUtils.h" // Debug Utils
#include "src/constants.h"
#include "src/ButtonHandler.h"
#include "src/encoderHandler.h"
#include "src/LCDHandler.h"
#include "src/LampControl.h"

//*****************************************************************************************//
//                                      Initial Setup
//*****************************************************************************************//
void setup()
{
  Serial.begin(115200);
  while (!Serial)
  time = micros();
    ; // Waiting for Serial Monitor
  randomSeed(analogRead(0));
  //  RTC.begin(DateTime(__DATE__, __TIME__));
  testLCD();
  initializeLCD();
  showInitScreen();
  initializeButtons();
  initializeEncoder();
  testEnlargerLamp();
}

/**
 *                                      MAIN LOOP
 */

void loop() {
    // Handle input from buttons and rotary encoder
    inputHandler();

    // Check if exposure is in progress
    if (startExposure) {
        handleEnlargerLamp(); // Manage relay operation during exposure
    } else {
        handleEncoderInput(); // Read rotary encoder input for timer adjustment
    }

    // Update the big digit display on the LCD if timer value changes
    updateTimerDisplay();

}