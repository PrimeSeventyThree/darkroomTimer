/*
 * File: timerLogic.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Tuesday, 31st December 2024 12:55:53 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Tuesday, 7th January 2025 12:06:41 pm
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

#include "constants.h"
#include "timerLogic.h"
#include <EEPROM.h>

void initializeTimerLogic() {
    // Restore stored timer delay from EEPROM
    EEPROM.get(eeAddress, storedTimerDelay);
    timerDelay = storedTimerDelay;
}

void updateTimerLogic() {
    if (startExposure) {
        digitalWrite(RELAY_PIN, HIGH);
        timerDelay -= 100;  // Decrement timer
        if (timerDelay <= 0) {
            timerDelay = storedTimerDelay;
            digitalWrite(RELAY_PIN, LOW);
            startExposure = false;
        }
    }
    checkAndWriteEEPROM(timerDelay); // Write to EEPROM if needed
}

void increaseTimerDelay() {
    if (timerDelay + 100 <= MAX_TIMER_DELAY) {
        timerDelay += 100;
    }
}

void decreaseTimerDelay() {
    if (timerDelay - 100 >= 0) {
        timerDelay -= 100;
    }
}

void setTimerDelay(long delay) {
    timerDelay = delay;
}

void checkAndWriteEEPROM(long delay) {
    unsigned long currentMillis = millis();

    // Write only if the value has changed and debounce time has passed
    if (delay != storedTimerDelay && currentMillis - lastEEPROMWrite > EEPROM_WRITE_DELAY) {
        EEPROM.put(0, delay);
        storedTimerDelay = delay;
        lastEEPROMWrite = currentMillis;
        Serial.print("EEPROM updated: ");
        Serial.println(delay);
    }
}
