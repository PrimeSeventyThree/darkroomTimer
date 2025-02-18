/*
 * File: ButtonHandler.h
 * Project: Darkroom Enlarger Timer
 * File Created: Thursday, 18th April 2024 2:17:14 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Monday, 17th February 2025 11:20:55 pm
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

#ifndef ButtonHandler_h
#define ButtonHandler_h

#include <Arduino.h>

// --- Buttons Configuration ---
constexpr uint8_t TIMER_BUTTON_PIN = 6;           // Timer start button
constexpr uint8_t ROTARY_ENCODER_BUTTON_PIN = 4;  // Rotary encoder's push button (resets timer to 0)
 
/**
 * @brief Represents the state of a button with debouncing logic.
 * 
 * This structure is used to manage the state of a button, including
 * debouncing to prevent false triggers due to noise. It keeps track
 * of the last debounce time, the debounce delay, and the button's
 * last and current states.
 * 
 * @var pressStartTime The last time the button state was toggled.
 * @var debounceDelay The delay in milliseconds to debounce the button.
 * @var lastDebounceTime The last time the button was toggled.
 * @var lastButtonState The previous state of the button.
 * @var currentButtonState The current state of the button.
 */
 struct ButtonState {
    unsigned long pressStartTime = 0;  // Stores the last time the button was pressed
    const unsigned long debounceDelay = 50;  // Debounce delay in milliseconds
    unsigned long lastDebounceTime = 0;  // The last time the output pin was toggled
    bool lastButtonState = HIGH;  // The previous reading from the input pin
    bool currentButtonState = HIGH;  // Current state of the button
    bool buttonIsPressed = false;       // Tracks the button state

};
void initializeButtons();
bool checkButtonState(uint8_t buttonPin, ButtonState& state);
void inputHandler();

#endif
