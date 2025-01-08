/*
 * File: lcdHandler.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Tuesday, 31st December 2024 2:55:26 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Wednesday, 8th January 2025 7:02:28 am
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

#include "lcdHandler.h"
#include "constants.h"

const byte segmentPatterns[7][8] PROGMEM = {
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

const byte bigNumbers[][30] PROGMEM = { // organized by row
    //       0                 1                 2                 3                 4                 5                 6                 7                 8                 9
    {0x01, 0x02, 0x03, 0x01, 0x02, 0xFE, 0x01, 0x02, 0x03, 0x01, 0x02, 0x03, 0x02, 0xFE, 0x02, 0x02, 0x02, 0x02, 0x01, 0x02, 0x03, 0x02, 0x02, 0x02, 0x01, 0x02, 0x03, 0x01, 0x02, 0x03},
    {0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0x01, 0x02, 0xFF, 0xFE, 0x02, 0xFF, 0xFF, 0x02, 0xFF, 0xFF, 0x02, 0x02, 0xFF, 0x02, 0x03, 0xFE, 0x01, 0x07, 0xFF, 0x02, 0xFF, 0xFF, 0xFE, 0xFF},
    {0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFE, 0xFE, 0xFF, 0xFE, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0x04, 0x06, 0xFF},
    {0x04, 0x06, 0x05, 0xFE, 0x06, 0xFE, 0x06, 0x06, 0x06, 0x04, 0x06, 0x05, 0xFE, 0xFE, 0x06, 0x04, 0x06, 0x05, 0x04, 0x06, 0x05, 0xFE, 0x06, 0xFE, 0x04, 0x06, 0x05, 0xFE, 0xFE, 0x06}};

byte col, row, nb = 0, bc = 0; // general
byte bb[8];                    // byte buffer for reading from PROGMEM

extern LiquidCrystal_I2C lcd;


/**
 * @brief Returns the number of bytes currently free in RAM.
 *
 * @return the number of free bytes in RAM
 */
int freeRam()
{
  extern char __bss_end;
  extern char *__brkval;

  int free_memory;
  if (reinterpret_cast<int>(__brkval) == 0)
  {
    // If heap is not being used, free memory is the space between the end of the static data
    // and the start of the stack
    free_memory = reinterpret_cast<int>(&free_memory) - reinterpret_cast<int>(&__bss_end);
  }
  else
  {
    // If heap is being used, free memory is the space between the end of the heap and the start of the stack
    free_memory = reinterpret_cast<int>(&free_memory) - reinterpret_cast<int>(__brkval);
  }

  return free_memory;
}


/**
 * @brief Prints a digit on an LCD screen.
 *
 * @param digit A digit to print.
 * @param leftAdjust A left adjustment of a digit.
 */
void printNum(byte digit, byte leftAdjust)
{
  for (byte row = 0; row < 4; row++)
  {
    lcd.setCursor(leftAdjust, row);
    for (byte col = digit * 3; col < digit * 3 + 3; col++)
    {
      lcd.write(pgm_read_byte(&bigNumbers[row][col]));
    }
  }
}

/**
 * @brief Erases a digit from an LCD screen.
 *
 * @param leftAdjust A left adjustment of a digit.
 */
void eraseNum(byte leftAdjust)
{
  for (byte row = 0; row < 4; row++)
  {
    lcd.setCursor(leftAdjust, row);
    for (byte col = leftAdjust; col < leftAdjust + 3; col++)
    {
      lcd.print(F(" "));
    }
  }
}

/**
 * @brief Prints a colon  ( : ) on an LCD screen.
 *
 * @param leftAdjust A left adjustment of a colon.
 */
void printColon(byte leftAdjust)
{
  for (byte row = 0; row < 4; row++)
  {
    lcd.setCursor(leftAdjust, row);
    if (row == 1 || row == 2)
      lcd.print(F("."));
    else
      lcd.print(F(" "));
  }
}

/**
 * @brief Prints a dot on an LCD screen.
 *
 * @param leftAdjust A left adjustment of a dot.
 */
void printDot(byte leftAdjust)
{
  for (byte row = 0; row < 4; row++)
  {
    lcd.setCursor(leftAdjust, row);
    if (row == 3)
      lcd.print(F("."));
    else
      lcd.print(F(" "));
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
void showBigDigit(uint8_t digit, uint8_t position) {
    if (digit > 9) return; // Ensure the digit is valid

    // Iterate over each row of the big digit
    for (uint8_t row = 0; row < 4; row++) {
        lcd.setCursor(position, row); // Set cursor to the appropriate position

        // Write the corresponding custom characters to the LCD
        for (uint8_t col = 0; col < 3; col++) {
            uint8_t charIndex = pgm_read_byte(&bigNumbers[row][digit * 3 + col]);
            lcd.write(charIndex);
        }
    }
}

/**
 * @brief Clears a large digit from the LCD display.
 * 
 * This function iterates over the rows and columns of a specified
 * position on the LCD to replace the existing digit with spaces,
 * effectively clearing it from the display.
 * 
 * @param position The starting column position for the digit to clear.
 */
void clearBigDigit(uint8_t position) {
    // Iterate over each row of the digit area
    for (uint8_t row = 0; row < 4; row++) {
        lcd.setCursor(position, row); // Set cursor to the appropriate position

        // Replace the digit with spaces
        for (uint8_t col = 0; col < 3; col++) {
            lcd.print(F(" "));
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
  lcd.begin(20, 4);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Darkroom Timer"));

  // Create custom characters in the LCD's memory
    for (byte i = 0; i < NUM_CUSTOM_CHARS; i++)
      {
        memcpy_P(bb, segmentPatterns[i], 8);
        lcd.createChar(i + 1, bb);
      }
}

/**
 * @brief Displays the initialization screen on the LCD and logs available memory.
 * 
 * This function sets up the LCD to show a dot and the text "SEC" at specified positions.
 * It also outputs a message to the serial monitor indicating that the Darkroom Timer
 * is initialized and prints the available memory in bytes.
 */
void showInitScreen()
{
  printDot(LCD_OFFSET + 7);
  lcd.setCursor(LCD_OFFSET + 12, 3);
  lcd.print(F("SEC"));
  Serial.print("\nDarkroom Timer is initialized. Available memory is: ");
  Serial.print(freeRam());
  Serial.println(" bytes");
}

/**
 * @brief Displays the splash screen for the Darkroom Exposure Timer.
 * 
 * This function clears the LCD screen and prints the title "DARKROOM" and 
 * "EXPOSURE TIMER" on the first two lines. It then shows the last stored 
 * delay in seconds on the third line, followed by the build version, 
 * revision number, and available free RAM in bytes on the fourth line. 
 * The splash screen is displayed for 2 seconds before the screen is cleared.
 */
void displaySplashScreen() {
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print(F("DARKROOM"));
    lcd.setCursor(3, 1);
    lcd.print(F("EXPOSURE TIMER"));

    // Display the last stored delay on one line
    lcd.setCursor(3, 2); // Adjust for proper alignment
    lcd.print(F("Last Delay: "));
    lcd.print(storedTimerDelay / 1000); // Convert to seconds
    lcd.print(F(" sec"));

    lcd.setCursor(6, 3); // Position for version and memory info
    lcd.print(F("V"));
    lcd.print(BUILD_VERSION);
    lcd.print(F("."));
    lcd.print(REVISION_NUMBER);
    lcd.print(F(" "));
    lcd.print(freeRam());
    lcd.print(F("B"));

    delay(2000); // Adjust delay for better readability
    lcd.clear();
}


/**
 * @brief Displays the hundreds, tens, and units digits of a given delay value on an LCD.
 *
 * The function extracts the hundreds, tens, and units digits from the provided
 * delay value and displays each digit on the LCD using the `printNum` function.
 * The digits are displayed in their respective positions: hundreds at position 0,
 * tens at position 6, and units at position 12.
 *
 * @param delay The delay value from which the digits are extracted and displayed.
 */
void displayBigNumbers(long delay) {
    // Split delay into individual digits
    byte digits[3] = {0};
    digits[0] = (delay / 10000) % 10;        // Hundreds
    digits[1] = (delay / 1000) % 10;         // Tens
    digits[2] = (delay / 100) % 10;          // Units

    // Display each digit on the LCD
    printNum(digits[0], 0);   // Hundreds position
    printNum(digits[1], 6);   // Tens position
    printNum(digits[2], 12);  // Units position
}

/**
 * @brief Updates the timer display on the LCD by calculating and displaying the 
 * digits representing the remaining time. The function calculates the 
 * tens, ones, and tenths place digits from the timer delay and updates 
 * the display only if the seconds remaining have changed. It uses 
 * `showBigDigit` to display non-zero digits and `clearBigDigit` to clear 
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
            showBigDigit(thirdDigit, LCD_OFFSET);
        } else {
            clearBigDigit(LCD_OFFSET);
        }

        // Update second digit
        if (secondDigit != 0 || thirdDigit != 0) {
            showBigDigit(secondDigit, LCD_OFFSET + 4);
        } else {
            clearBigDigit(LCD_OFFSET + 4);
        }

        // Update first digit
        showBigDigit(firstDigit, LCD_OFFSET + 8);

        // Update previous values
        previousSe = se;
        previousThirdDigit = thirdDigit;
        previousSecondDigit = secondDigit;
        previousFirstDigit = firstDigit;
    }
}

