/*
 * File: encoderHandler.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Tuesday, 31st December 2024 2:55:45 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Wednesday, 8th January 2025 7:04:10 am
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

#include "encoderHandler.h"
#include "constants.h"

MD_REncoder rotaryEncoder(ROTARY_ENCODER_PIN_A, ROTARY_ENCODER_PIN_B);

/**
 * Initializes the rotary encoder hardware.
 *
 * This function sets up the rotary encoder by calling its begin method,
 * and outputs a debug message indicating the initialization process.
 */
 void initializeEncoder() {
    DEBUG_PRINT("Initializing Encoder...");
    rotaryEncoder.begin();
}

/**
 * @brief Reads the rotary encoder input and adjusts the timer delay based on the rotation direction and speed.
 * 
 * The function reads the direction of the rotary encoder and modifies the timer delay accordingly.
 * If speed adjustment is enabled and the encoder speed exceeds a predefined limit, the increment
 * value is adjusted proportionally. The timer delay is decreased when the encoder is rotated
 * counterclockwise and increased when rotated clockwise, within the bounds of the maximum delay.
 * Debug information is printed to indicate the direction and current timer delay.
 */
void handleEncoderInput() {
    uint8_t direction = rotaryEncoder.read();
    uint32_t tempIncrement = TimerConfig::INCREMENT; // Local copy of increment value

    if (direction) {
        // Adjust increment based on speed if enabled
        if (ENABLE_SPEED && rotaryEncoder.speed() > ROTARY_ENCODER_SPEED_LIMIT) {
            tempIncrement *= rotaryEncoder.speed();
        }

        // Handle direction-specific logic
        if (direction == DIR_CCW) {
            // Decrease timer delay when moving counterclockwise
            timerDelay = timerDelay > tempIncrement ? timerDelay - tempIncrement : 0;
            DEBUG_PRINT("CCW ");
        } else if (direction == DIR_CW) {
            // Increase timer delay when moving clockwise
            timerDelay = timerDelay + tempIncrement < TimerConfig::MAX_DELAY ? timerDelay + tempIncrement : TimerConfig::MAX_DELAY;
            DEBUG_PRINT("CW ");
        }

        // Debug output
        DEBUG_PRINT("timerDelay: ");
        DEBUG_PRINT(timerDelay);
    }
}

