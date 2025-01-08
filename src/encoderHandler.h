/*
 * File: encoderHandler.h
 * Project: Darkroom Enlarger Timer
 * File Created: Tuesday, 31st December 2024 2:55:37 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Tuesday, 7th January 2025 7:33:37 pm
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

#ifndef ENCODER_HANDLER_H
#define ENCODER_HANDLER_H

#include <MD_REncoder.h>

// Rotary Encoder Configuration
#define ROTARY_ENCODER_PIN_A 3 // left pin (A)
#define ROTARY_ENCODER_PIN_B 2 // right pin (B)

#define ROTARY_ENCODER_SPEED_LIMIT 2 // the rotary encoder speed threshold above which the increment value is multiplied by the current speed value. This is to avoid the encoder to be too sensitive when setting the timer delay.

extern MD_REncoder rotaryEncoder;

void initializeEncoder();
void handleEncoderInput();
// bool encoderInputDetected();

#endif
