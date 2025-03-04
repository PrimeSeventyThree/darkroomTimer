/*
* File: darkroom_timer_test.ino
* Project: Darkroom Enlarger Timer
* File Created: Tuesday, 31st December 2024 12:35:55 pm
* Author: Andrei Grichine (andrei.grichine@gmail.com)
* -----
* Last Modified: Wednesday, 26th February 2025 9:58:28 pm
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

#include <ArduinoUnit.h>
#include <Arduino.h>
#include "src/constants.h"

#ifdef ENABLE_TESTS
#include "test/encoderHandler_test.cpp"
#endif

void setup() {
    Serial.begin(115200);
    while (!Serial);

    Serial.println("Starting Unit Tests...");
}

void loop() {
#ifdef ENABLE_TESTS
    Test::run(); // Executes all tests
#endif
}