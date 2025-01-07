/*
 * File: ButtonHandler.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Tuesday, 7th January 2025 9:57:16 am
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Tuesday, 7th January 2025 4:52:48 pm
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

#include "ButtonHandler.h"
#include "constants.h"
#include "LampControl.h"
#include <Arduino.h>
#include <EEPROM.h> // to store/read data in EEPROM

ButtonState encoderButtonState;
ButtonState timerButtonState;

void initializeButtons() {
    pinMode(TIMER_BUTTON_PIN, INPUT_PULLUP);
    pinMode(ROTARY_ENCODER_BUTTON_PIN, INPUT_PULLUP);
        // Restore stored timer delay from EEPROM
    EEPROM.get(eeAddress, storedTimerDelay);
    timerDelay = storedTimerDelay;
}

/**
 * @brief Function to check the button state and manage debouncing
 */
bool checkButtonState(int buttonPin, ButtonState& state) {
    bool stateChanged = false;
    int reading = digitalRead(buttonPin);
    if (reading != state.lastButtonState) {
        state.lastDebounceTime = millis();
    }
    if ((millis() - state.lastDebounceTime) > state.debounceDelay) {
        if (reading != state.currentButtonState) {
            state.currentButtonState = reading;
            stateChanged = true;
        }
    }
    state.lastButtonState = reading;
    return stateChanged;
}

/**
 * @brief Handles the input from the rotary encoder and the timer button.
 */

void inputHandler() {
    if (checkButtonState(ROTARY_ENCODER_BUTTON_PIN, encoderButtonState)) {
        if (encoderButtonState.currentButtonState == LOW) {
            timerDelay = 0;
            turnEnlargerLampOff();
        }
    }

    if (checkButtonState(TIMER_BUTTON_PIN, timerButtonState)) {
        if (timerButtonState.currentButtonState == LOW) {
            timerButtonIsPressed = true;
            timerButtonState.lastDebounceTime = millis();
        } else if (timerButtonIsPressed) {
            // Retrieve the last timer delay from EEPROM for comparison
            EEPROM.get(eeAddress, storedTimerDelay);
            if (timerDelay != storedTimerDelay) {
                EEPROM.put(eeAddress, timerDelay);
                DEBUG_PRINT("EEPROM updated with new timer delay.");
            }

            turnOnEnlargerLamp = true; // Signals to turn on the lamp
            time = micros(); // Added to reset timer if stopped and then started
            timerButtonIsPressed = false;

            if ((millis() - timerButtonState.lastDebounceTime) < TURN_ENLARGER_LAMP_ON_DELAY) {
                DEBUG_PRINT("Timer Button is Released.");
                DEBUG_PRINT("Starting Exposure.");
                startExposure = true; // Signals relay to start exposure
                turnManuallyOnEnlargerLamp = true;
                turnEnlargerLampOn();
            } else {
                startExposure = false;
                DEBUG_PRINT("Timer Button is Released after a manual delay.");
                DEBUG_PRINT(String(millis() - timerButtonState.lastDebounceTime));
                turnManuallyOnEnlargerLamp = !turnManuallyOnEnlargerLamp;
                DEBUG_PRINT("turnManuallyOnEnlargerLamp : ");
                DEBUG_PRINT(String(turnManuallyOnEnlargerLamp));
                digitalWrite(MANUAL_LIGHT_PIN, turnManuallyOnEnlargerLamp ? HIGH : LOW);
            }
        }
    }
}



/**
 * @brief Function to handle specific logic when the timer button state changes
*/

void handleTimerButtonLogic() {
    if (timerButtonIsPressed) {
        if (timerButtonState.currentButtonState == HIGH) {
            storedTimerDelay = timerDelay;
            EEPROM.put(eeAddress, storedTimerDelay);
            turnOnEnlargerLamp = true;
            time = micros();
            timerButtonIsPressed = false;

            if ((millis() - timerButtonState.lastDebounceTime) < TURN_ENLARGER_LAMP_ON_DELAY) {
                startExposure = true;
                turnManuallyOnEnlargerLamp = true;
            } else {
                startExposure = false;
                turnManuallyOnEnlargerLamp = !turnManuallyOnEnlargerLamp;
                digitalWrite(MANUAL_LIGHT_PIN, LOW);
                turnEnlargerLampOn();
            }
        } else if ((millis() - timerButtonState.lastDebounceTime) > TURN_ENLARGER_LAMP_ON_DELAY) {
            digitalWrite(MANUAL_LIGHT_PIN, HIGH);
        }
    }
}