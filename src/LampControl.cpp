/*
 * File: LampControl.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Tuesday, 7th January 2025 9:31:59 am
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Tuesday, 7th January 2025 1:26:10 pm
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
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;

/**
 * @brief Tests the enlarger lamp
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
 */
void turnEnlargerLampOn() {
    if (!turnManuallyOnEnlargerLamp) {
        turnEnlargerLampOff();
    }
    if (turnOnEnlargerLamp) {
        Serial.println(F("Turning enlarger lamp ON"));
        digitalWrite(RELAY_PIN, HIGH);
        lcd.noBacklight();
        turnOnEnlargerLamp = false;
    }
}

/**
 * @brief Handles the Enlarger Lamp.
 */
void handleEnlargerLamp() {
    // Turn on the enlarger lamp if it's not already manually turned off
    if (!turnManuallyOnEnlargerLamp) {
        turnEnlargerLampOn();
    }

    // Check if the exposure timer has reached its duration
    if ((micros() - time) >= DURATION) {
        // Reset the timer if micros() has rolled over
        if (micros() < time) {
            time = micros();
        } else {
            time += DURATION;
        }

        // Decrease the timer delay
        if (timerDelay > TIMER_INCREMENT) {
            timerDelay -= TIMER_INCREMENT;
        } else {
            timerDelay = storedTimerDelay; // Reset to stored value
            turnEnlargerLampOff();
            startExposure = false; // Stop exposure
        }
    }
}

/**
 * @brief Turns the enlarger lamp off.
 */

void turnEnlargerLampOff() {
    Serial.println(F("Turning enlarger lamp OFF"));
    startExposure = false;
    turnOnEnlargerLamp = false;
    turnManuallyOnEnlargerLamp = false;
    timerButtonIsPressed = false;
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(MANUAL_LIGHT_PIN, LOW);
    lcd.backlight();
}
