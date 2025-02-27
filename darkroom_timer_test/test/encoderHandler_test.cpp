/*
 * File: encoderHandler_test.cpp
 * Project: Darkroom Enlarger Timer
 * File Created: Wednesday, 26th February 2025 10:10:45 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Wednesday, 26th February 2025 11:04:20 pm
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
#include "../src/encoderHandler.h"
#include "../src/constants.h"

#ifdef ENABLE_TESTS

// Mock the rotaryEncoder class with a rotation sequence
class MockRotaryEncoder : public MD_REncoder {
    public:
        MockRotaryEncoder(uint8_t pinA, uint8_t pinB) : MD_REncoder(pinA, pinB) {}
    
        void setRotationSequence(const uint8_t seq[], size_t len) {
            rotationSequence = seq;
            sequenceLength = len;
            currentIndex = 0;
        }
    
        uint8_t read() {
            if (currentIndex < sequenceLength) {
                return rotationSequence[currentIndex++];
            } else {
                return DIR_NONE; // Or some default if sequence is exhausted
            }
        }
    
    private:
        const uint8_t* rotationSequence = nullptr;
        size_t sequenceLength = 0;
        size_t currentIndex = 0;
    };
    
    // Encoder Handler Tests
    test(EncoderButtonHandler_multiple_rotations) {
        // Create a local mock instance
        MockRotaryEncoder rotaryEncoder(ROTARY_ENCODER_PIN_A, ROTARY_ENCODER_PIN_B);
    
        // Define a rotation sequence (CW, CW, CCW, NONE)
        const uint8_t rotation[] = {DIR_CW, DIR_CW, DIR_CCW, DIR_NONE};
        int rotationArraySize = sizeof(rotation) / sizeof(rotation[0]);
        rotaryEncoder.setRotationSequence(rotation, rotationArraySize);
        
        // Call handleEncoderInput multiple times to simulate rotation
        for (int i = 0; i < rotationArraySize; ++i) { // Number of rotations equals the rotation array in the Mock
            handleEncoderInput();
        }
    
        // Assert that timerDelay is updated correctly based on the rotation sequence (1000+100+100-100 == 1100)
        assertEqual((long)timerDelay, (long)1100);
    }
    

#endif