/*
 * File: ButtonHandler.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Monday, 17th February 2025 11:11:12 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Monday, 24th February 2025 5:29:16 pm
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
#include "MemoryUtils.h"
#include "ButtonHandler.h"
#include "constants.h"
#include "LampControl.h"

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
    for (int index = 0; index < EEPROM_ADDRESS_RANGE; index++) {
        int currentEEPROMAddress = getNextEEPROMAddress();
        storedTimerDelay = readEEPROMWithRetry(currentEEPROMAddress);
        DEBUG_PRINTF("Restored timer delay %d from EEPROM at %d address", storedTimerDelay, currentEEPROMAddress);
        if (storedTimerDelay != -1) break;
      }
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
 * @brief Manages the lamp's lifecycle based on the desired action.
 *
 * For a short press (lampState true), the lamp is turned on (start exposure).
 * For a long press (lampState false), the manual lamp state is toggled and applied.
 *
 * @param lampPin The pin controlling the lamp.
 * @param lampState If true, the lamp is turned on; if false, the manual lamp state is toggled.
 */
 void manageLampLifeCycle(int lampPin, bool lampState) {
    startExposure = lampState;
    if (!lampState) {
        // For a long press, toggle the manual lamp state and use that.
        turnManuallyOnEnlargerLamp = !turnManuallyOnEnlargerLamp;
        lampState = turnManuallyOnEnlargerLamp;
    }
    digitalWrite(lampPin, lampState);
}

/**
 * @brief Processes the button release event, handling both short and long presses.
 */
void processButtonRelease() {
    unsigned long currentMillis = millis();
    // Update stored timer delay and prepare the enlarger lamp.
    // Check if enough time has passed since the last EEPROM write
    if (currentMillis - lastEEPROMWrite >= TimerConfig::EEPROM_WRITE_DELAY) {
        // Check if the timerDelay has changed since the last EEPROM write
        if (timerDelay != storedTimerDelay) {
            int nextAddress = getNextEEPROMAddress();
            if (writeEEPROMWithRetry(nextAddress, timerDelay)) {
                DEBUG_PRINTF("EEPROM updated at %d address. Stored delay is %d ms.", nextAddress, timerDelay);
                eeAddress = nextAddress; // Update current EEPROM address
                storedTimerDelay = timerDelay; // Update stored delay
                lastEEPROMWrite = currentMillis; // Update last write time
                
            } else {
                DEBUG_PRINT("EEPROM write failed, skipping address.");
            }
        } else {
             DEBUG_PRINT("EEPROM not updated: value unchanged");
        }
    } else {
        DEBUG_PRINT("EEPROM not updated: too soon");
    }
    storedTimerDelay = timerDelay;
    turnOnEnlargerLamp = true;
    time = micros();

    unsigned long elapsedTime = currentMillis - timerButtonState.pressStartTime;
    timerButtonState.buttonIsPressed = false;

    // Execute actions based on press duration.
    if (elapsedTime < TimerConfig::TURN_ENLARGER_LAMP_ON_DELAY) {
        // Short press: start exposure.
        manageLampLifeCycle(MANUAL_LIGHT_PIN, true);
    } else {
        // Long press: toggle manual lamp control.
        manageLampLifeCycle(MANUAL_LIGHT_PIN, false);
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
}

/**
 * @brief Processes input from the timer button to control the enlarger lamp.
 *
 * This function debounces the timer button and handles the start and release events.
 * It distinguishes between short and long presses to either start an exposure or toggle
 * the manual lamp mode. Minimal error handling is included given production Arduino constraints.
 */
 void handleTimerButtonPress() {
    // Check for a stable state change using debounce.
    if (!debounceButton(TIMER_BUTTON_PIN, timerButtonState))
        return;

    // Handle button press start.
    if (timerButtonState.currentButtonState == LOW && !timerButtonState.buttonIsPressed) {
        timerButtonState.buttonIsPressed = true;
        timerButtonState.pressStartTime = millis();
        // (Optional) Debug: Serial.print("Button pressed at "); Serial.println(timerButtonState.pressStartTime);
    }
    // Handle button release.
    else if (timerButtonState.currentButtonState == HIGH && timerButtonState.buttonIsPressed) {
        processButtonRelease();
    }
}
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
