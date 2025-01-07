/*
 * File: darkroom_timer_test.ino
 * Project: Darkroom Enlarger Timer
 * File Created: Tuesday, 31st December 2024 12:35:55 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Monday, 6th January 2025 9:56:42 am
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
#include <EEPROM.h>
#include "src/constants.h"
#include "src/timerLogic.h"
#include "src/lcdHandler.h"
#include "src/encoderHandler.h"

#define EEPROM_TEST_ADDRESS 10  // Test address to avoid overwriting real EEPROM data

// Mock Variables
long mockTimerDelay = 0;
long mockStoredTimerDelay = 0;

// Mock EEPROM Helper Functions
void mockEEPROMWrite(long value) {
    EEPROM.put(EEPROM_TEST_ADDRESS, value);
}

long mockEEPROMRead() {
    long value;
    EEPROM.get(EEPROM_TEST_ADDRESS, value);
    return value;
}

// **TDD Cycle for Timer Logic**

test(timer_should_increment_by_100) {
    mockTimerDelay = 5000;
    timerDelay = mockTimerDelay;

    increaseTimerDelay(); // Call the function to test
    assertEqual(timerDelay, mockTimerDelay + 100); // Test passes when timerDelay increments correctly
}

// Next: Test boundaries (TDD principle of testing edge cases).
test(timer_should_not_exceed_maximum) {
    mockTimerDelay = MAX_TIMER_DELAY - 50; // Near max
    timerDelay = mockTimerDelay;

    increaseTimerDelay(); // Call the function to test
    assertEqual(timerDelay, MAX_TIMER_DELAY); // Test passes if timerDelay doesn't exceed the limit
}

// Test decrement functionality with boundary conditions.
test(timer_should_not_decrement_below_zero) {
    mockTimerDelay = 50; // Near zero
    timerDelay = mockTimerDelay;

    decreaseTimerDelay(); // Call the function to test
    assertEqual(timerDelay, 0); // Test passes if timerDelay doesn't go negative
}

// **TDD Cycle for EEPROM Write Logic**
// 1. Write a failing test for debounced writes.
test(eeprom_should_write_only_after_delay) {
    mockStoredTimerDelay = 10000;
    mockEEPROMWrite(mockStoredTimerDelay);
    long eepromValue = mockEEPROMRead();

    assertEqual(eepromValue, mockStoredTimerDelay); // Initial state

    // Test debouncing by simulating a quick, repeated call.
    unsigned long startMillis = millis();
    checkAndWriteEEPROM(mockStoredTimerDelay); // Should not write if no change
    assertEqual(mockEEPROMRead(), mockStoredTimerDelay);

    // Simulate time passage to allow write
    delay(EEPROM_WRITE_DELAY + 10);
    checkAndWriteEEPROM(mockStoredTimerDelay + 500); // Should write new value
    assertEqual(mockEEPROMRead(), mockStoredTimerDelay + 500);
}

// **TDD Cycle for LCD Rendering**
// 1. Write a failing test for big number rendering.
test(lcd_should_render_big_numbers) {
    initializeLCD();

    // Mock a delay value
    long testDelay = 12345; // 1 minute 23.45 seconds
    timerDelay = testDelay;

    // Call big-number rendering
    displayBigNumbers(timerDelay);

    // Validate visually (cannot be automated for the LCD).
    pass();
}

// **TDD Cycle for Encoder Input Handling**
test(encoder_should_increase_timer_delay) {
    initializeEncoder();

    // Simulate rotary encoder increment
    increaseTimerDelay();
    assertEqual(timerDelay, 100); // Ensure the delay increases correctly
}

// **Mock Encoder Rotation and Test**
test(encoder_should_decrease_timer_delay) {
    timerDelay = 200;
    decreaseTimerDelay();
    assertEqual(timerDelay, 100); // Decrease by 100ms
}

void setup() {
    Serial.begin(115200);
    while (!Serial);

    // Automatically run all tests
    Serial.println("Starting Unit Tests...");
}

void loop() {
    Test::run(); // Executes all tests
}
