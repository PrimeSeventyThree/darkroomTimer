/*
 * File: LampControl.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Tuesday, 7th January 2025 9:31:59 am
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Monday, 17th February 2025 11:21:09 pm
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



#include "LampControl.h"
#include "constants.h"
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;

/**
 * @brief Tests the enlarger lamp by toggling the relay pin.
 *
 * This function sets the relay pin to output mode and turns it on for 1 second
 * to test the enlarger lamp. After the delay, it turns the relay off and sets
 * the manual light pin to output mode.
 */
void testEnlargerLamp() {
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH);
    delay(1000);
    digitalWrite(RELAY_PIN, LOW);
    pinMode(MANUAL_LIGHT_PIN, OUTPUT);
}
 
/**
 *  @brief Turns the enlarger lamp on if it is not manually turned off.
 * 
 * This function first checks if the lamp is manually turned off. If not, it
 * ensures the lamp is turned off before proceeding. If the lamp is set to be
 * turned on, it activates the lamp by setting the relay pin high and turns off
 * the LCD backlight. The flag to turn on the lamp is then reset.
 */
void turnEnlargerLampOn() {
    if (turnOnEnlargerLamp) {
        DEBUG_PRINT("Turning enlarger lamp ON");
        digitalWrite(RELAY_PIN, HIGH);
        lcd.noBacklight();
        turnOnEnlargerLamp = false;
    }
}

/**
 * @brief Handles the operation of the enlarger lamp based on manual control and timer settings.
 * 
 * This function turns on the enlarger lamp if it is not manually turned off. It checks
 * if the exposure timer has reached its configured duration and adjusts the timer delay
 * accordingly. If the timer delay reaches its minimum increment, the lamp is turned off
 * and exposure is stopped.
 */
void handleEnlargerLamp() {
    // Turn on the enlarger lamp if it's not already manually turned off
    // if (!turnManuallyOnEnlargerLamp) {
        turnEnlargerLampOn();
    // }

    // Check if the exposure timer has reached its TimerConfig::DURATION
    unsigned long currentTime = micros();
    if ((currentTime - time) >= TimerConfig::DURATION) {
        // Reset the timer if micros() has rolled over
        time = (currentTime < time) ? currentTime : time + TimerConfig::DURATION;

        // Decrease the timer delay
        if (timerDelay > TimerConfig::INCREMENT) {
            timerDelay -= TimerConfig::INCREMENT;
        } else {
            timerDelay = storedTimerDelay; // Reset to stored value
            turnEnlargerLampOff();
            startExposure = false; // Stop exposure
        }
    }
}

/**
 * @brief Turns off the enlarger lamp and resets related states.
 *
 * This function deactivates the enlarger lamp by setting the relay and manual light pins to LOW,
 * and turns on the LCD backlight. It also resets various control flags, including those for
 * starting exposure, turning on the enlarger lamp, manual lamp control, and the timer button state.
 */
void turnEnlargerLampOff() {
    //DEBUG_PRINT("Turning enlarger lamp OFF");
    startExposure = false;
    turnOnEnlargerLamp = false;
    turnManuallyOnEnlargerLamp = false;
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(MANUAL_LIGHT_PIN, LOW);
    lcd.backlight();
}
