/*
 * File: LCDHandler.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Monday, 17th February 2025 12:58:56 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Tuesday, 18th February 2025 6:36:15 am
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

#include "lcdHandler.h"
#include "constants.h"
#include "MemoryUtils.h"

// Define the desired LCD layout:
#define SELECTED_LCD_LAYOUT LCDLayout4x20 // see constants.h for definitions

const uint8_t segmentPatterns[7][8] PROGMEM = {
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F}, // char 1: top left triangle
    {0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F}, // char 2: top horizontal block
    {0x00, 0x00, 0x00, 0x00, 0x10, 0x1C, 0x1E, 0x1F}, // char 3: top right triangle
    {0x1F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00}, // char 4: bottom left triangle
    {0x1F, 0x1E, 0x1C, 0x10, 0x00, 0x00, 0x00, 0x00}, // char 5: bottom right triangle
    {0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00}, // char 6: bottom horizontal block
    {0x1F, 0x1F, 0x1E, 0x1C, 0x18, 0x18, 0x10, 0x10}  // char 7: middle right triangle (used to render a part of "7" character)
                                                      // room for another one!
};
// 0xFE - 5x8 segment is OFF
// 0xFF - 5x8 segment is ON

const int NUM_CUSTOM_CHARS = sizeof(segmentPatterns) / sizeof(segmentPatterns[0]); // NUM_CUSTOM_CHARS = 7

/*
EXAMPLE
The zero is made of 3x4 segments on 4x20 LCD, so we need to use 3x4=12 bytes to store the pattern for "0". 
Using patterns defined in segmentPatterns array above we would use:

on line 1: char 1, char 2, char 3 (0x01, 0x02, 0x03)
on line 2: ON, OFF, ON            (0xFF, 0xFE, 0xFF)
on line 3: ON, OFF, ON            (0xFF, 0xFE, 0xFF)
on line 4: char 4, char 6, char 5 (0x04, 0x06, 0x05)

See these numbers in the array below. The array is organized by row.

Here is the pattern for "0" character on 4x20 LCD:
..... ..... .....
..... ..... .....
..... ..... .....
..... ..... .....
....# ##### #....
..### ##### ###..
.#### ##### ####.
##### ##### #####

##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####

##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####

##### ##### #####
.#### ##### ####.
..### ##### ###..
....# ##### #....
..... ..... .....
..... ..... .....
..... ..... .....
..... ..... .....

*/

const uint8_t bigNumbers[][30] PROGMEM = { // organized by row
    //       0                 1                 2                 3                 4                 5                 6                 7                 8                 9
    {0x01, 0x02, 0x03, 0x01, 0x02, 0xFE, 0x01, 0x02, 0x03, 0x01, 0x02, 0x03, 0x02, 0xFE, 0x02, 0x02, 0x02, 0x02, 0x01, 0x02, 0x03, 0x02, 0x02, 0x02, 0x01, 0x02, 0x03, 0x01, 0x02, 0x03},
    {0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0x01, 0x02, 0xFF, 0xFE, 0x02, 0xFF, 0xFF, 0x02, 0xFF, 0xFF, 0x02, 0x02, 0xFF, 0x02, 0x03, 0xFE, 0x01, 0x07, 0xFF, 0x02, 0xFF, 0xFF, 0xFE, 0xFF},
    {0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFE, 0xFE, 0xFF, 0xFE, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0x04, 0x06, 0xFF},
    {0x04, 0x06, 0x05, 0xFE, 0x06, 0xFE, 0x06, 0x06, 0x06, 0x04, 0x06, 0x05, 0xFE, 0xFE, 0x06, 0x04, 0x06, 0x05, 0x04, 0x06, 0x05, 0xFE, 0x06, 0xFE, 0x04, 0x06, 0x05, 0xFE, 0xFE, 0x06}};

    uint8_t col, row, nb = 0, bc = 0; // general
    uint8_t bb[8];                    // byte buffer for reading from PROGMEM

extern LiquidCrystal_I2C lcd;

/**
 * @brief Prints a colon  ( : ) on an LCD screen.
 *
 * @param leftAdjust A left adjustment of a colon.
 */
void printColon(uint8_t leftAdjust) {
  for (uint8_t row = 0; row < 4; row++) {
    lcd.setCursor(leftAdjust, row);
    lcd.print((row == 1 || row == 2) ? F(".") : F(" "));
  }
}

/**
 * @brief Prints a dot on an LCD screen.
 *
 * @param leftAdjust A left adjustment of a dot.
 */
void printDot(uint8_t leftAdjust) {
  for (uint8_t row = 0; row < 4; row++) {
    lcd.setCursor(leftAdjust, row);
    lcd.print((row == 3) ? F(".") : F(" "));
  }
}

/**
 * @brief Displays a large representation of a digit on the LCD screen.
 * 
 * This function renders a specified digit (0-9) as a large character
 * on the LCD, starting at the given column position. It ensures the
 * digit is valid and uses custom characters to create the display.
 * 
 * @param digit The digit to display, ranging from 0 to 9.
 * @param position The starting column position on the LCD for the digit.
 */
void drawOrEraseBigDigit(uint8_t position, uint8_t digit = 0, bool erase = false) {
    if (digit > 9) return; // Ensure the digit is valid

    // Iterate over each row of the big digit
    for (uint8_t row = 0; row < 4; ++row) {
        lcd.setCursor(position, row); // Set cursor to the appropriate position
        if(erase) {
          lcd.print(F("   ")); // Print three spaces (erase a row) in one call
        } else {
          // Write the corresponding custom characters to the LCD
          for (uint8_t col = 0; col < 3; ++col) {
              uint8_t charIndex = pgm_read_byte(&bigNumbers[row][digit * 3 + col]);
              lcd.write(charIndex);
        }
      }
    }
}

/**
 * @brief Tests the LCD by blinking its backlight three times.
 * 
 * This function performs a quick test of the LCD's backlight by
 * turning it on and off three times, with a delay of 250 milliseconds
 * between each state change. The function concludes with the backlight
 * remaining on.
 */
 void testLCD() {
  // ------- Quick 3 blinks of an LCD's backlight  -------------
  for (int i = 0; i < 3; i++)
  {
    lcd.backlight();
    delay(250);
    lcd.noBacklight();
    delay(250);
  }
  lcd.backlight(); // finish with backlight on
}

/**
 * @brief Initializes the LCD by setting up the display, turning on the backlight,
 *        clearing the screen, and creating custom characters in the LCD's memory.
 */
void initializeLCD() {
  lcd.begin(SELECTED_LCD_LAYOUT::LCD_COLS, SELECTED_LCD_LAYOUT::LCD_ROWS);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);

  // Create custom characters in the LCD's memory
    for (uint8_t i = 0; i < NUM_CUSTOM_CHARS; i++)
      {
        memcpy_P(bb, segmentPatterns[i], 8);
        lcd.createChar(i + 1, bb);
      }
}

/**
 * @brief Displays static text on the LCD screen. Decimal point and "SEC" text in a fixed position. The position is specific to the 4x20 LCD.
 * 
 */
void displayStaticText() {
      printDot(SELECTED_LCD_LAYOUT::LCD_OFFSET + SELECTED_LCD_LAYOUT::STATIC_DOT_POSITION);
    lcd.setCursor(SELECTED_LCD_LAYOUT::LCD_OFFSET + SELECTED_LCD_LAYOUT::STATIC_SEC_TEXT_POSITION, SELECTED_LCD_LAYOUT::LCD_ROW_FOUR); //cursor is in the last 4x20 LCD row 
    lcd.print(F("SEC"));
}

/**
 * @brief Returns the formatted time in seconds from the stored timer delay.
 * 
 * This function converts the stored timer delay in milliseconds to seconds
 * and formats the result as a string with one decimal place. The formatted
 * time is returned as a String object.
 * 
 * @return A formatted string representing the time in seconds.
 */
String getFormattedTime() {
 float seconds = storedTimerDelay / 1000.0;
  char buffer[8];
  dtostrf(seconds, 3 /*min width*/, 1 /*decimal precision*/, buffer);
  return String(buffer);
}
/**
 * @brief Displays the splash screen for the Darkroom Exposure Timer.
 * 
 * This function clears the LCD screen and prints the title "DARKROOM" and 
 * "EXPOSURE TIMER" on the first two lines. It then shows the last stored 
 * delay in seconds on the third line, followed by the build version, 
 * revision number, and available free RAM in bytes on the fourth line. 
 * The splash screen is displayed for 2 seconds before the screen is cleared.
 * 
 * IMPORTANT 
 *
 * Global flash string constants are now defined using PROGMEM rather than the F() macro,
 * which avoids compile-time errors from using statement-expressions outside of functions.
 * This change ensures that strings like "EXPOSURE TIMER" reside in flash memory to save SRAM,
 * and they are properly cast to __FlashStringHelper* when printing to the LCD.
 */
void displaySplashScreen() { 
    lcd.clear();
    uint8_t len = strlen_P(SplashScreen::LINE_ONE_TEXT);
    lcd.setCursor((SELECTED_LCD_LAYOUT::LCD_COLS-len)/2, SELECTED_LCD_LAYOUT::LCD_ROW_ONE);
    lcd.print(reinterpret_cast<const __FlashStringHelper*>(SplashScreen::LINE_ONE_TEXT));
    len = strlen_P(SplashScreen::LINE_TWO_TEXT);
    lcd.setCursor((SELECTED_LCD_LAYOUT::LCD_COLS-len)/2, SELECTED_LCD_LAYOUT::LCD_ROW_TWO);
    lcd.print(reinterpret_cast<const __FlashStringHelper*>(SplashScreen::LINE_TWO_TEXT));

    // Display the last stored delay on one line
    lcd.setCursor(SELECTED_LCD_LAYOUT::LAST_DELAY_COL, SELECTED_LCD_LAYOUT::LCD_ROW_THREE); // Adjust for proper alignment
    lcd.print(F("Last Delay: "));
    lcd.print(getFormattedTime());
    lcd.print(F("s"));

    lcd.setCursor(6, SELECTED_LCD_LAYOUT::LCD_ROW_FOUR); // Position for version and memory info
    lcd.print(F("V"));
    lcd.print(reinterpret_cast<const __FlashStringHelper*>(BUILD_VERSION));
    lcd.print(F("."));
    lcd.print(reinterpret_cast<const __FlashStringHelper*>(REVISION_NUMBER));
    lcd.print(F(" "));
    lcd.print(freeRam());
    lcd.print(F("B"));

    delay(2000); // Adjust delay for better readability
    lcd.clear();
}

/**
 * @brief Updates the timer display on the LCD by calculating and displaying the 
 * digits representing the remaining time. The function calculates the 
 * tens, ones, and tenths place digits from the timer delay and updates 
 * the display only if the seconds remaining have changed. It uses 
 * `drawOrEraseBigDigit` to display non-zero digits and `clearBigDigit` to clear 
 * positions when necessary.
 */
void updateTimerDisplay() {
    // Calculate digits
    uint8_t se = timerDelay / 100; // Seconds remaining
    uint8_t thirdDigit = timerDelay / 10000;                             // 10s place
    uint8_t secondDigit = (timerDelay % 10000) / 1000;                  // 1s place
    uint8_t firstDigit = (timerDelay % 1000) / TimerConfig::INCREMENT;               // 0.1s place

    // Update only if seconds remaining have changed
    static uint8_t previousSe = 255, previousThirdDigit = 255, previousSecondDigit = 255, previousFirstDigit = 255;

    if (se != previousSe) {
        // Update third digit
        if (thirdDigit != 0) {
          drawOrEraseBigDigit(SELECTED_LCD_LAYOUT::LCD_OFFSET + SELECTED_LCD_LAYOUT::THIRD_BIG_DIGIT_OFFSET, thirdDigit);
        } else {
          drawOrEraseBigDigit(SELECTED_LCD_LAYOUT::LCD_OFFSET + SELECTED_LCD_LAYOUT::THIRD_BIG_DIGIT_OFFSET, true);
        }

        // Update second digit
        if (secondDigit != 0 || thirdDigit != 0) {
          drawOrEraseBigDigit(SELECTED_LCD_LAYOUT::LCD_OFFSET + SELECTED_LCD_LAYOUT::SECOND_BIG_DIGIT_OFFSET, secondDigit);
        } else {
          drawOrEraseBigDigit(SELECTED_LCD_LAYOUT::LCD_OFFSET + SELECTED_LCD_LAYOUT::SECOND_BIG_DIGIT_OFFSET, true);
        }

        // Update first digit
        drawOrEraseBigDigit(SELECTED_LCD_LAYOUT::LCD_OFFSET + SELECTED_LCD_LAYOUT::FIRST_BIG_DIGIT_OFFSET, firstDigit);

        // Update previous values
        previousSe = se;
        previousThirdDigit = thirdDigit;
        previousSecondDigit = secondDigit;
        previousFirstDigit = firstDigit;
    }
}

