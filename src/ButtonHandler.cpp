/*
 * File: ButtonHandler.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Tuesday, 7th January 2025 9:57:16 am
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Sunday, 12th January 2025 10:01:51 pm
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

/**
 * Initializes button pins and restores the stored timer delay from EEPROM.
 *
 * Configures the timer and rotary encoder button pins as input with pull-up
 * resistors. Retrieves the previously stored timer delay value from EEPROM
 * and assigns it to the timerDelay variable.
 */
void initializeButtons() {
    pinMode(TIMER_BUTTON_PIN, INPUT_PULLUP);
    pinMode(ROTARY_ENCODER_BUTTON_PIN, INPUT_PULLUP);
        // Restore stored timer delay from EEPROM
    EEPROM.get(eeAddress, storedTimerDelay);
    timerDelay = storedTimerDelay;
}

/**
 * @brief Debounces the button input to prevent false triggering due to noise.
 * 
 * This function reads the current state of a button and determines if the 
 * state has changed after a specified debounce delay. It updates the button 
 * state and returns whether the state has changed.
 * 
 * @param buttonPin The pin number connected to the button.
 * @param state A reference to the ButtonState structure holding the button's 
 *              current and last states, as well as the last debounce time.
 * @return true if the button state has changed, false otherwise.
 */
bool debounceButton(uint8_t buttonPin, ButtonState& state) {
    // Check if the button pin is valid
    if (buttonPin >= NUM_DIGITAL_PINS) {
        return false; // Invalid pin number
    }
    // Read the current time once for consistent time checks
    unsigned long currentMillis = millis();
    // Read the current state of the button
    int newReading = digitalRead(buttonPin);
    bool stateChanged = false;

    // If the button state has changed, reset the debouncing timer
    if (newReading != state.lastButtonState) {
        state.lastDebounceTime = currentMillis;
    }

    // Check if the button state is stable for longer than the debounce delay
    if ((currentMillis - state.lastDebounceTime) > state.debounceDelay && newReading != state.currentButtonState) {
        // Update the current button state and mark state as changed
        state.currentButtonState = newReading;
        stateChanged = true;
    }

    // Save the current reading as the last button state
    state.lastButtonState = newReading;
    return stateChanged;
}

/**
 * @brief Processes input from the rotary encoder and timer button to control the enlarger lamp.
 * 
 * This function checks the states of the rotary encoder button and the timer button.
 * It debounces the button inputs and manages the logic for turning the enlarger lamp on or off.
 * The function also handles storing the timer delay in EEPROM and manages manual lamp control.
 */
void handleEncoderButton() {
    if (debounceButton(ROTARY_ENCODER_BUTTON_PIN, encoderButtonState) && encoderButtonState.currentButtonState == LOW) {
        timerDelay = 0;
        timerButtonState.buttonIsPressed = false;
        turnEnlargerLampOff();
    }
}

/**
 * @brief Processes input from the timer button to control the enlarger lamp.
 * 
 * This function checks the state of the timer button and debounces the button input.
 * It manages the logic for turning the enlarger lamp on or off based on the button press
 * duration and current state. The function also handles storing the timer delay in EEPROM
 * and manages manual lamp control.
 */
void handleTimerButtonPress() {
    // 1. Check if the stable state changed
    if (debounceButton(TIMER_BUTTON_PIN, timerButtonState)) {

        // 2. If it just went LOW → button press start
        if (timerButtonState.currentButtonState == LOW && !timerButtonState.buttonIsPressed) {
            timerButtonState.buttonIsPressed = true;
            timerButtonState.pressStartTime = millis();

            // (Optional) debug
            DEBUG_PRINT("Timer Button Pressed at " + String(timerButtonState.pressStartTime) + " ms");

        // 3. If it just went HIGH → button release
        } else if (timerButtonState.currentButtonState == HIGH && timerButtonState.buttonIsPressed) {
                // Update the stored timer delay and turn on the enlarger lamp
                storedTimerDelay = timerDelay;
                EEPROM.put(eeAddress, storedTimerDelay);
                turnOnEnlargerLamp = true;
                time = micros();

                timerButtonState.buttonIsPressed = false;                unsigned long pressEndTime = millis();
                unsigned long elapsedTime = pressEndTime - timerButtonState.pressStartTime;

                // Print debug
                DEBUG_PRINT("Timer Button Released at " + String(pressEndTime) +
                            " ms; Elapsed: " + String(elapsedTime) + " ms");

                // 3a. Short Press
                if (elapsedTime < TimerConfig::TURN_ENLARGER_LAMP_ON_DELAY) {
                    DEBUG_PRINT("Short Press → Starting Exposure");
                    startExposure = true;
                    turnManuallyOnEnlargerLamp = true;
                    digitalWrite(MANUAL_LIGHT_PIN, HIGH);

                // 3b. Long Press
                } else {
                    DEBUG_PRINT("Long Press → Toggle Manual Lamp");
                    startExposure = false; // Cancel exposure
                    turnManuallyOnEnlargerLamp = !turnManuallyOnEnlargerLamp;
                    digitalWrite(MANUAL_LIGHT_PIN, turnManuallyOnEnlargerLamp ? HIGH : LOW);
                    turnEnlargerLampOn();
                }
            }
        }
    }

    // 4. (Optional) Handle the case: user is STILL holding the button
    //    for immediate feedback, like your original code setting
    //    "Timer Button is Pressed longer than a manual delay."
    // if (timerButtonState.buttonIsPressed) {
    //     unsigned long now = millis();
    //     if ((now - timerButtonState.pressStartTime) > TimerConfig::TURN_ENLARGER_LAMP_ON_DELAY) {
    //         DEBUG_PRINT("Timer Button Held > 2s → Lamp ON or Manual Indication");
    //         digitalWrite(MANUAL_LIGHT_PIN, HIGH);
    //     }
    // }


/**
 * @brief Handles the input from the rotary encoder and timer button.
 * 
 * This function processes the input from the rotary encoder and timer button
 * by calling the corresponding handler functions. It also manages the long
 * press action for the timer button.
 */
void inputHandler() {
    handleEncoderButton();
    handleTimerButtonPress();
}
