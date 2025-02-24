/*
 * File: MemoryUtils.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Monday, 17th February 2025 9:22:34 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Monday, 24th February 2025 5:26:05 pm
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

 #include <EEPROM.h>
 #include "MemoryUtils.h"
 #include "constants.h"
 #include "LCDHandler.h"

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
 * @brief Gets the next EEPROM address for wear leveling.
 *
 * This function increments the current EEPROM address index and wraps around
 * to the start address when the end of the address range is reached.
 *
 * @return The next EEPROM address to use.
 */
int getNextEEPROMAddress() {
  // currentEEPROMAddressIndex = (currentEEPROMAddressIndex + 1) % EEPROM_ADDRESS_RANGE;
  return EEPROM_START_ADDRESS;
}

/**
* @brief Writes a value to EEPROM with retry logic to handle bad blocks.
*
* This function attempts to write a value to the specified EEPROM address.
* If the write fails (value read back is incorrect), it increments the bad
* blocks count and returns false. The function also handles skipping bad
* blocks and displaying a warning message on the LCD if too many bad blocks
* are encountered.
*
* @param address The EEPROM address to write to.
* @param value The value to write.
* @return True if the write was successful, false otherwise.
*/
bool writeEEPROMWithRetry(int address, long value) {
  const int MAX_RETRIES = 3;
  for (int retry = 0; retry < MAX_RETRIES; ++retry) {
      EEPROM.put(address, value);
      long readValue;
      EEPROM.get(address, readValue);

      if (readValue == value) {
          return true; // Write successful
      }
      delay(10); // Small delay before retry
  }

  // Write failed after multiple retries
  badBlocksCount++;
  DEBUG_PRINTF("EEPROM write failed at address: %d", address);

  if (badBlocksCount > MAX_BAD_BLOCKS) {
    EEPROM_FAILED = true; //set the flag to display user a message
  }
  return false;
}

/**
* @brief Reads a value to EEPROM with retry logic to handle bad blocks.
*
* This function attempts to read a value to the specified EEPROM address.
* If the read fails (value read back is incorrect), it increments the bad
* blocks count and returns false. The function also handles skipping bad
* blocks and sets a flag to display a warning message on the LCD
* if too many bad blocks are encountered.
*
* @param address The EEPROM address to read from.
* @return timer delay stored in EEPROM, otherwise -1
*/
int readEEPROMWithRetry(int address) {
  const int MAX_RETRIES = 3;
  for (int retry = 0; retry < MAX_RETRIES; ++retry) {
      long readValue;
      EEPROM.get(address, readValue);
      if ((readValue >= 0 && readValue <= TimerConfig::MAX_DELAY) || readValue != EEPROM_INIT_VALUE) { // Check if the value is within the valid range and not the default value
        DEBUG_PRINTF("Read timer delay %d from EEPROM at address: %d", readValue, address);
        return readValue;
      }
      delay(10); // Small delay before retry
  }

  // Write failed after multiple retries
  badBlocksCount++;
  DEBUG_PRINTF("EEPROM read failed at address: %d", address);

  if (badBlocksCount > MAX_BAD_BLOCKS) {
    EEPROM_FAILED = true; //set the flag to display user a message
  }
  return -1;
}

/**
 * @brief Initializes EEPROM if it hasn't been set up already.
 *
 * Checks for a magic number in EEPROM. If the magic number is missing or incorrect,
 * it writes default values for all settings and then writes the magic number.
 */
 void initializeEEPROM() {
  long magicValue;
  EEPROM.get(MAGIC_ADDRESS, magicValue);
  
  if (magicValue != EEPROM_MAGIC) {
    for (int index = 0; index < EEPROM_ADDRESS_RANGE; index++) {
      int currentEEPROMAddress = getNextEEPROMAddress();
      writeEEPROMWithRetry(currentEEPROMAddress,EEPROM_INIT_VALUE);
    }
      // Write the magic number to mark EEPROM as initialized.
      EEPROM.put(MAGIC_ADDRESS, EEPROM_MAGIC);
  }
}


