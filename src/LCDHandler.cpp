/*
 * File: LCDHandler.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Monday, 17th February 2025 12:58:56 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Tuesday, 25th February 2025 12:28:35 am
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

#include <avr/pgmspace.h>
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
    {0x01, 0x02, 0x03, 0xFE, 0x01, 0x02, 0x01, 0x02, 0x03, 0x01, 0x02, 0x03, 0x02, 0xFE, 0x02, 0x02, 0x02, 0x02, 0x01, 0x02, 0x03, 0x02, 0x02, 0x02, 0x01, 0x02, 0x03, 0x01, 0x02, 0x03},
    {0xFF, 0xFE, 0xFF, 0xFE, 0xFE, 0xFF, 0x01, 0x02, 0xFF, 0xFE, 0x02, 0xFF, 0xFF, 0x02, 0xFF, 0xFF, 0x02, 0x02, 0xFF, 0x02, 0x03, 0xFE, 0x01, 0x07, 0xFF, 0x02, 0xFF, 0xFF, 0xFE, 0xFF},
    {0xFF, 0xFE, 0xFF, 0xFE, 0xFE, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFE, 0xFE, 0xFF, 0xFE, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0x04, 0x06, 0xFF},
    {0x04, 0x06, 0x05, 0xFE, 0xFE, 0x06, 0x06, 0x06, 0x06, 0x04, 0x06, 0x05, 0xFE, 0xFE, 0x06, 0x04, 0x06, 0x05, 0x04, 0x06, 0x05, 0xFE, 0x06, 0xFE, 0x04, 0x06, 0x05, 0xFE, 0xFE, 0x06}};

    uint8_t col, row, nb = 0, bc = 0; // general
    uint8_t bb[8];                    // byte buffer for reading from PROGMEM

extern LiquidCrystal_I2C lcd;

/**
 * @brief Prints a special character on an LCD screen.
 *
 * @param charType The type of character to print (':', '.', etc.)
 * @param leftAdjust A left adjustment of the character.
 */
 /**
 * @brief Prints a special character on an LCD screen.
 *
 * @param charType The type of character to print (':', '.', etc.)
 * @param leftAdjust A left adjustment of the character.
 */
void printSpecialChar(char charType, uint8_t leftAdjust) {
  // Define patterns for different special characters
  // Each array represents which rows should have a dot for the character
  const bool colonPattern[4] = {false, true, true, false}; // Colon pattern
  const bool dotPattern[4] = {false, false, false, true};  // Dot pattern
  
  // Select the appropriate pattern based on character type
  const bool* pattern = NULL;
  
  switch (charType) {
    case ':':
      pattern = colonPattern;
      break;
    case '.':
      pattern = dotPattern;
      break;
    // Add more cases here for future special characters
    // case 'x':
    //   pattern = xPattern;
    //   break;
    default:
      // Default to empty pattern if character not recognized
      static const bool emptyPattern[4] = {false, false, false, false};
      pattern = emptyPattern;
      break;
  }
  
  // Print the character based on the selected pattern
  for (uint8_t row = 0; row < 4; row++) {
    lcd.setCursor(leftAdjust, row);
    lcd.print(pattern[row] ? F(".") : F(" "));
  }
}

/**
 * @brief Draws or erases a big digit on the LCD with optimized memory access.
 * 
 * This function renders a specified digit (0-9) as a large character
 * on the LCD, starting at the given column position. It ensures the
 * digit is valid and uses custom characters to create the display.
 * 
 * @param position The column position on the LCD to start drawing
 * @param digit The digit (0-9) to display
 * @param erase Whether to erase instead of draw the digit
 */
 void drawOrEraseBigDigit(uint8_t position, uint8_t digit, bool erase = false) {
  // Skip validation when erasing or for valid digits
  if (!erase && digit > 9) return;
  
  // Calculate base offset for the digit in bigNumbers array
  const uint8_t digitOffset = digit * 3;
  
  // Process each row
  for (uint8_t row = 0; row < 4; ++row) {
      // Position cursor only once per row
      lcd.setCursor(position, row);
      
      if (erase) {
          // Erase the entire row with one operation
          lcd.print(F("   "));
      } else {
          // Get pointer to row data for this digit in program memory
          const uint8_t* rowPtr = &bigNumbers[row][digitOffset];
          
          // Efficient single-loop character writing with direct pgm_read
          for (uint8_t col = 0; col < 3; ++col) {
              uint8_t charIndex = pgm_read_byte(&rowPtr[col]);
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
  printSpecialChar('.',SELECTED_LCD_LAYOUT::LCD_OFFSET + SELECTED_LCD_LAYOUT::STATIC_DOT_POSITION);
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
 char* getFormattedTime() {
  static char buffer[8]; // Static ensures the buffer persists after the function returns
  float seconds = storedTimerDelay / 1000.0;
  dtostrf(seconds, 3 /*min width*/, 1 /*decimal precision*/, buffer);
  return buffer;
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
    lcd.setCursor(floor((SELECTED_LCD_LAYOUT::LCD_COLS-len)/2), SELECTED_LCD_LAYOUT::LCD_ROW_ONE);
    lcd.print(reinterpret_cast<const __FlashStringHelper*>(SplashScreen::LINE_ONE_TEXT));
    len = strlen_P(SplashScreen::LINE_TWO_TEXT);
    lcd.setCursor(floor((SELECTED_LCD_LAYOUT::LCD_COLS-len)/2), SELECTED_LCD_LAYOUT::LCD_ROW_TWO);
    lcd.print(reinterpret_cast<const __FlashStringHelper*>(SplashScreen::LINE_TWO_TEXT));

    // Display the last stored delay on one line
    lcd.setCursor(SELECTED_LCD_LAYOUT::LAST_DELAY_COL, SELECTED_LCD_LAYOUT::LCD_ROW_THREE); // Adjust for proper alignment
    lcd.print(F("Last Delay: "));
    lcd.print(getFormattedTime());
    lcd.print(F("s"));

    int freeRamText = freeRam();
    size_t bufferSize = snprintf(NULL, 0, "v%d.%d %dB", BUILD_VERSION, REVISION_NUMBER, freeRamText)+1; // passing NULL as a first argument to snprintf returns a size the buffer needed to store the string.
    char  *buffer = malloc(bufferSize);
    snprintf(buffer, bufferSize, "v%d.%d %dB", BUILD_VERSION, REVISION_NUMBER, freeRamText);
    len = strlen(buffer);
    lcd.setCursor(floor((SELECTED_LCD_LAYOUT::LCD_COLS-len)/2), SELECTED_LCD_LAYOUT::LCD_ROW_FOUR); // Position for version and memory info
    lcd.print(buffer);
    free(buffer);
    delay(5000); // Adjust delay for better readability. 5 sec seems long enough
    lcd.clear();
}

/**
 * @brief Updates the timer display on the LCD with optimized rendering.
 * 
 * Calculates digit values and renders them with leading zero suppression.
 * Only updates when the displayed time has changed to minimize LCD operations.
 */
 void updateTimerDisplay() {
  // Constants for display management
  constexpr uint8_t EMPTY_DIGIT = 0xFF;
  
  // Calculate time value in deciseconds (0.1s)
  uint16_t deciseconds = timerDelay / 100;
  
  // Only update if the displayed time has changed
  static uint16_t previousDeciseconds = UINT16_MAX;

  // Get positions from layout constants
  const uint8_t positions[3] = {
    SELECTED_LCD_LAYOUT::FIRST_BIG_DIGIT_OFFSET,
    SELECTED_LCD_LAYOUT::SECOND_BIG_DIGIT_OFFSET,
    SELECTED_LCD_LAYOUT::THIRD_BIG_DIGIT_OFFSET
  };
  
  if (deciseconds != previousDeciseconds) {
      DEBUG_PRINTF("Updating display for %d.%ds\n", deciseconds/10, deciseconds%10);
      
      // Extract individual digits (more efficiently)
      uint8_t firstDigit = deciseconds % 10;                // 0.1s place
      uint8_t secondDigit = (deciseconds / 10) % 10;        // 1s place
      uint8_t thirdDigit = deciseconds / 100;               // 10s place
      
      // Store current digit values to track when a redraw is needed
      static uint8_t displayedDigits[3] = {EMPTY_DIGIT, EMPTY_DIGIT, EMPTY_DIGIT};
      
      // Create a local digit array for current values
      uint8_t digits[3] = {firstDigit, secondDigit, thirdDigit};
      
      // Handle leading zeros and update each digit position
      bool isLeadingZero = true;
      
      // Process digits from most to least significant (right to left on display)
      for (int8_t i = 2; i >= 0; i--) {
          // Determine if this is a leading zero (except for the least significant digit)
          bool shouldErase = (isLeadingZero && digits[i] == 0 && i > 0);
          
          // Only redraw if the digit value or visibility has changed
          bool digitChanged = (digits[i] != displayedDigits[i]);
          bool wasBlank = (displayedDigits[i] == EMPTY_DIGIT); // Using EMPTY_DIGIT (0xFF) as a marker for blank
          
          if (digitChanged || (shouldErase && !wasBlank) || (!shouldErase && wasBlank)) {
              // Draw or erase this digit
              drawOrEraseBigDigit(
                  SELECTED_LCD_LAYOUT::LCD_OFFSET + positions[i],
                  digits[i],
                  shouldErase
              );
              
              // Update displayed digit tracking
              displayedDigits[i] = shouldErase ? 0xFF : digits[i];
          }
          
          // Once we hit a non-zero digit, stop suppressing zeros
          if (digits[i] != 0) {
              isLeadingZero = false;
          }
      }
      
      // Store current time for next comparison
      previousDeciseconds = deciseconds;
  }
}

/**
 * @brief Displays an error message on the LCD indicating EEPROM failure.
 */
 void displayEEPROMError() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("EEPROM Failure!"));
  lcd.setCursor(0, 1);
  lcd.print(F("Replace ASAP!"));
  delay(2000); // Display for 2 seconds
}
