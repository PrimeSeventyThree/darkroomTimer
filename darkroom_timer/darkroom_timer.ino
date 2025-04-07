/*
 * File: darkroom_timer.ino
 * Project: Darkroom Enlarger Timer
 * File Created: Monday, 17th February 2025 12:58:56 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Monday, 7th April 2025 4:38:11 pm
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
 * @brief Implements a darkroom enlarger timer using an Arduino.
 * 
 * This program controls a darkroom enlarger timer, utilizing a rotary encoder
 * for input and an LCD for display. It manages the exposure timing and updates
 * the display with large digits. The setup function initializes the necessary
 * hardware components, while the loop function handles user input and manages
 * the exposure process.
 * 
 * The code is based on the BIG FONT (4-line) LCD CHARACTERS project by Adrian Jones,
 * which can be found at http://woodsgood.ca/projects/2015/02/27/4-line-lcd-big-numbers/.
 * 
 * Requirements:
 * - MD_REncoder: Rotary Encoder Library (https://github.com/MajicDesigns/MD_REncoder)
 * - New-LiquidCrystal: LCD Library for Arduino and Chipkit (https://github.com/fmalpartida/New-LiquidCrystal)
 * 
 * Debugging can be enabled by defining the DEBUG macro.
 * 
 */

#include "src/DebugUtils.h" // Debug Utils
#include "src/constants.h"
#include "src/ButtonHandler.h"
#include "src/encoderHandler.h"
#include "src/LCDHandler.h"
#include "src/LampControl.h"
#include "src/MemoryUtils.h"
 
#define SERIAL_BAUD 115200
/**
 * @brief Initializes the system components and prepares the environment.
 * 
 * This function sets up the serial communication, seeds the random number generator,
 * and initializes various hardware components such as the LCD, buttons, and encoder.
 * It also tests the LCD and enlarger lamp to ensure they are functioning correctly.
 */
void setup() {
  Serial.begin(SERIAL_BAUD);
  while (!Serial);  // Waiting for Serial Monitor
  randomSeed(analogRead(0));
  initializeButtons();
  testLCD();
  initializeLCD();
  displaySplashScreen();
  initializeEncoder();
  testEnlargerLamp();
  displayStaticText();
  restoreEEPROMAddress(); // Restore address from EEPROM!
}
void loop() {
  // Handle input from buttons and rotary encoder
  inputHandler();

  // Check if exposure is in progress
  if (startExposure) {
      handleEnlargerLamp(); // Manage relay operation during exposure
  }
  else {
      handleEncoderInput(); // Read rotary encoder input for timer adjustment
  }

  // Update the big digit display on the LCD if timer value changes
  updateTimerDisplay();
  if (EEPROM_FAILED) displayEEPROMError();

}