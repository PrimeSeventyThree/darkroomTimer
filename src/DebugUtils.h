/*
 * File: DebugUtils.h
 * Project: Darkroom Enlarger Timer
 * File Created: Monday, 17th February 2025 12:58:56 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Monday, 24th February 2025 2:53:23 pm
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
 * @brief Debug utilities for Arduino projects.
 *
 * This header provides two macros for debug logging:
 *
 * 1. DEBUG_PRINT:
 *    - Prints a single string along with a detailed debug header.
 *    - The header includes:
 *         - Elapsed time in milliseconds (millis())
 *         - Compile time (__TIME__)
 *         - Function name (__PRETTY_FUNCTION__)
 *         - Source file and line number (__FILE__ and __LINE__)
 *
 * 2. DEBUG_PRINTF:
 *    - A printf-style macro that supports multiple parameters.
 *    - It formats a message using printf-style formatting into a temporary buffer,
 *      then prints the debug header (as above) followed by the formatted message.
 *    - If the formatted message does not include a newline ('\n'), one is appended automatically.
 *
 * Detailed behavior of DEBUG_PRINTF:
 *    - A temporary character buffer of 128 bytes (__dbg_buf) is created.
 *    - The message is formatted into __dbg_buf using snprintf, with the format string and any
 *      additional parameters passed to the macro.
 *    - The macro then prints the following to the Serial interface:
 *         - The current millis() value, followed by "ms : "
 *         - The compile time (__TIME__), followed by a space.
 *         - The function name (__PRETTY_FUNCTION__), followed by a space.
 *         - The file name (__FILE__), a colon, and the line number (__LINE__), followed by a space.
 *         - The formatted message stored in __dbg_buf.
 *    - Finally, the macro checks if the formatted message contains a newline character.
 *      If it does not, it appends a newline by calling Serial.println().
 *
 * @note The macros are only active when the DEBUG flag is defined. If DEBUG is not defined,
 *       both macros will resolve to no-operations.
 *
 * @param fmt The format string (as used in printf-style formatting).
 * @param ... Additional arguments matching the format specifiers in fmt.
 *
 * @example
 * @code
 *   // Example usage in an Arduino sketch:
 *   #define DEBUG
 *   #include "debugutils.h"
 *
 *   void setup() {
 *       Serial.begin(9600);
 *
 *       // Using DEBUG_PRINT to log a simple message.
 *       DEBUG_PRINT("System initializing...");
 *
 *       // Using DEBUG_PRINTF to log a formatted message.
 *       int temperature = 25;
 *       int humidity = 60;
 *       DEBUG_PRINTF("Temperature: %d°C, Humidity: %d%%", temperature, humidity);
 *   }
 *
 *   void loop() {
 *       // Main loop code.
 *   }
 * @endcode
 *
 * @see DEBUG_PRINT for logging a simple string with the same debug header.
 */

#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#ifdef DEBUG

#define DEBUG_PRINT(str)     \
   Serial.print(millis());     \
   Serial.print("ms : ");     \
   Serial.print(__TIME__);     \
   Serial.print(" ");     \
   Serial.print(__PRETTY_FUNCTION__);     \
   Serial.print(' ');     \
   Serial.print(__FILE__);     \
   Serial.print(':');     \
   Serial.print(__LINE__);     \
   Serial.print(' ');     \
   Serial.println(str);

#define DEBUG_PRINTF(fmt, ...) do {                             \
    char __dbg_buf[128];                                        \
    /* Format the string into our temporary buffer */         \
    snprintf(__dbg_buf, sizeof(__dbg_buf), (fmt), ##__VA_ARGS__); \
    /* Print debug header */                                    \
    Serial.print(millis());                                     \
    Serial.print("ms : ");                                      \
    Serial.print(__TIME__);                                     \
    Serial.print(" ");                                          \
    Serial.print(__PRETTY_FUNCTION__);                          \
    Serial.print(' ');                                          \
    Serial.print(__FILE__);                                     \
    Serial.print(':');                                          \
    Serial.print(__LINE__);                                     \
    Serial.print(' ');                                          \
    /* Print the formatted message */                           \
    Serial.print(__dbg_buf);                                    \
    /* If no newline is found in the message, add one */        \
    if (strchr(__dbg_buf, '\n') == NULL) {                      \
        Serial.println();                                       \
    }                                                           \
} while (0)

#else

#define DEBUG_PRINT(str)
#define DEBUG_PRINTF(fmt, ...) do {} while (0)

#endif

#endif // DEBUGUTILS_H

