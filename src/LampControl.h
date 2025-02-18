/*
 * File: LampControl.h
 * Project: Darkroom Enlarger Timer
 * File Created: Monday, 17th February 2025 12:58:56 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Tuesday, 18th February 2025 12:27:30 am
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

#include <Arduino.h>
#ifndef LAMP_CONTROL_H
#define LAMP_CONTROL_H

/**
 * Relay Configuration
 *
 * Defines the pin numbers for controlling the relay and indicating manual light mode.
 * RELAY_PIN is used to control the enlarger lamp.
 * MANUAL_LIGHT_PIN is used as an indicator (LED) for manual light mode.
 */
 constexpr uint8_t RELAY_PIN = 7;                   // Relay pin to control the enlarger lamp
 constexpr uint8_t MANUAL_LIGHT_PIN = 8;            // Indicator pin for manual light mode

void testEnlargerLamp();
void turnEnlargerLampOn();
void handleEnlargerLamp();
void turnEnlargerLampOff();

#endif // LAMP_CONTROL_H
