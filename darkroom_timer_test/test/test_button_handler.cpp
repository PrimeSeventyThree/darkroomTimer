/*
 * File: test_button_handler.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Wednesday, 26th February 2025 9:34:26 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Wednesday, 26th February 2025 9:53:53 pm
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

#include <ArduinoUnit.h>
#include "../src/ButtonHandler.h"
#include "../src/constants.h"

 // Mock functions for testing buttons
 // Mock functions for testing encoder, mock the variables too
 
 long mockTimerDelay = 0; 
 namespace encoderHandler
 {

    void increaseTimerDelay() {
         mockTimerDelay = (mockTimerDelay + TimerConfig::INCREMENT < TimerConfig::MAX_DELAY) ? (mockTimerDelay + TimerConfig::INCREMENT) : TimerConfig::MAX_DELAY;
     }
 
     void decreaseTimerDelay() {
         mockTimerDelay = (mockTimerDelay > TimerConfig::INCREMENT) ? (mockTimerDelay - TimerConfig::INCREMENT) : 0;
     }
   long timerDelay = 0;
 }

 // Encoder Button Handler Tests
 test(EncoderButtonHandler_should_increment_timer_delay) {
     mockTimerDelay = 5000;
     encoderHandler::increaseTimerDelay();
     assertEqual(mockTimerDelay, 5100); // Make sure they match
 }
 
 test(EncoderButtonHandler_should_decrement_timer_delay) {
     mockTimerDelay = 5000;
     encoderHandler::decreaseTimerDelay();
     assertEqual(mockTimerDelay, 4900); // Make sure they match
 }
 
 test(EncoderButtonHandler_at_max_delay) {
    mockTimerDelay = 600000;
    encoderHandler::increaseTimerDelay();
     assertEqual(mockTimerDelay, 599000); // Make sure they match
 }
 
  // Mock functions for testing buttons
  namespace ButtonHandler
  {
    long timerDelay = 0;
    long storedTimerDelay = 0;
  
  }