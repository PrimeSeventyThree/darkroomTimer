/*
 * File: ButtonHandler.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Tuesday, 7th January 2025 9:57:16 am
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Tuesday, 7th January 2025 7:30:43 pm
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
 * @brief Function to debounce the button input
 * 
 * @param buttonPin The pin number of the button
 * @param state The state of the button
 * @return true
 * @return false 
 */

bool debounceButton(int buttonPin, ButtonState& state) {
    int reading = digitalRead(buttonPin);
    bool stateChanged = false;

    if (reading != state.lastButtonState) {
        state.lastDebounceTime = millis(); // Reset the debouncing timer
    }

    if ((millis() - state.lastDebounceTime) > DEBOUNCE_DELAY) {
        if (reading != state.currentButtonState) {
            state.currentButtonState = reading;
            stateChanged = true;
        }
    }

    state.lastButtonState = reading; // Save the reading
    return stateChanged;
}

/**
 * @brief Handles the input from the rotary encoder and the timer button.
 */

void inputHandler() {
    // Check the encoder button state
    if (debounceButton(ROTARY_ENCODER_BUTTON_PIN, encoderButtonState)) {
        if (encoderButtonState.currentButtonState == LOW) {
            timerDelay = 0;
            turnEnlargerLampOff();
        }
    }

    // Check the timer button state
    if (debounceButton(TIMER_BUTTON_PIN, timerButtonState)) {
        if (timerButtonState.currentButtonState == LOW) {
            timerButtonIsPressed = true;
            timerButtonState.lastDebounceTime = millis();
        }
    }

    // Handle the timer button logic when pressed
    if (timerButtonIsPressed) {
        if (timerButtonState.currentButtonState == HIGH) {
            storedTimerDelay = timerDelay;
            EEPROM.put(eeAddress, storedTimerDelay);
            turnOnEnlargerLamp = true; // Signals to turn on the lamp
            time = micros();          // Reset timer
            timerButtonIsPressed = false;

            if ((millis() - timerButtonState.lastDebounceTime) < TimerConfig::TURN_ENLARGER_LAMP_ON_DELAY) {
                DEBUG_PRINT("Timer Button is Released. Starting Exposure for " + String(timerDelay) + " milliseconds...");
                startExposure = true; // Relay control
                turnManuallyOnEnlargerLamp = true;
            } else {
                startExposure = false;
                DEBUG_PRINT("Timer Button is Released after a manual delay. Elapsed Time: " + String(millis() - timerButtonState.lastDebounceTime));
                turnManuallyOnEnlargerLamp = !turnManuallyOnEnlargerLamp;
                DEBUG_PRINT("turnManuallyOnEnlargerLamp: " + String(turnManuallyOnEnlargerLamp));
                digitalWrite(MANUAL_LIGHT_PIN, turnManuallyOnEnlargerLamp ? HIGH : LOW);
                turnEnlargerLampOn();
            }
        } else if ((millis() - timerButtonState.lastDebounceTime) > TimerConfig::TURN_ENLARGER_LAMP_ON_DELAY) {
            DEBUG_PRINT("Timer Button is Pressed longer than a manual delay."); 
            digitalWrite(MANUAL_LIGHT_PIN, HIGH);
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

            if ((millis() - timerButtonState.lastDebounceTime) < TimerConfig::TURN_ENLARGER_LAMP_ON_DELAY) {
                startExposure = true;
                turnManuallyOnEnlargerLamp = true;
            } else {
                startExposure = false;
                turnManuallyOnEnlargerLamp = !turnManuallyOnEnlargerLamp;
                digitalWrite(MANUAL_LIGHT_PIN, LOW);
                turnEnlargerLampOn();
            }
        } else if ((millis() - timerButtonState.lastDebounceTime) > TimerConfig::TURN_ENLARGER_LAMP_ON_DELAY) {
            digitalWrite(MANUAL_LIGHT_PIN, HIGH);
        }
    }
}