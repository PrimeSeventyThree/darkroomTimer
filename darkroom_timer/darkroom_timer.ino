/*
 * File: darkroom_timer.ino
 * Project: Darkroom Timer
 * File Created: Wednesday, 21st July 2021 10:40:30 am
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Thursday, 18th April 2024 1:55:46 pm
 * Modified By: Andrei Grichine (andrei.grichine@gmail.com>)
 * -----
 * Copyright 2019 - 2022, Prime73 Inc. MIT License
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

//********************************************************************************
//
//    DARKROOM EXPOSURE TIMER
//    Andrei Grichine, December 2017
//
//    this code is based on:
//    BIG FONT (4-line) LCD CHARACTERS
//    http://woodsgood.ca/projects/2015/02/27/4-line-lcd-big-numbers/
//    Adrian Jones, February 2015
//
//    REQUIREMENTS:
//    MD_REncoder - Rotary Encoder Library (https://github.com/MajicDesigns/MD_REncoder)
//    New-LiquidCrystal - LCD Library for Arduino and Chipkit (https://github.com/fmalpartida/New-LiquidCrystal)
//********************************************************************************

#define build 1
#define revision 6
//********************************************************************************

// #define DEBUG           // Comment out this line to disable all debug output before uploading final sketch to a board
#include "DebugUtils.h" // Debug Utils

#include <avr/pgmspace.h> // for memory storage in program space

#include <EEPROM.h> // to store/read data in EEPROM
#include <Wire.h>

// Extra libraries to handle LCD
#include <LCD.h>
#include <LiquidCrystal_I2C.h> // library for I2C interface
#include <MD_REncoder.h>       // Rotary Encoder

#define I2C_ADDR 0x27 // address found from an I2C scanner
#define RS_pin 0      // pin configuration for LCM1602 interface module
#define RW_pin 1
#define EN_pin 2
#define BACK_pin 3
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7

#define LED_OFF 0
#define LED_ON 1

const byte custom[][8] PROGMEM = {
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F}, // char 1: bottom right triangle
    {0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F}, // char 2: bottom block
    {0x00, 0x00, 0x00, 0x00, 0x10, 0x1C, 0x1E, 0x1F}, // char 3: bottom left triangle
    {0x1F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00}, // char 4: top right triangle
    {0x1F, 0x1E, 0x1C, 0x10, 0x00, 0x00, 0x00, 0x00}, // char 5: top left triangle
    {0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00}, // char 6: upper block
    {0x1F, 0x1F, 0x1E, 0x1C, 0x18, 0x18, 0x10, 0x10}  // char 7: full top right triangle
                                                      // room for another one!
};

const byte bn[][30] PROGMEM = { // organized by row
    //         0               1               2               3               4              5               6                7               8               9
    {0x01, 0x02, 0x03, 0x01, 0x02, 0xFE, 0x01, 0x02, 0x03, 0x01, 0x02, 0x03, 0x02, 0xFE, 0x02, 0x02, 0x02, 0x02, 0x01, 0x02, 0x03, 0x02, 0x02, 0x02, 0x01, 0x02, 0x03, 0x01, 0x02, 0x03},
    {0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0x01, 0x02, 0xFF, 0xFE, 0x02, 0xFF, 0xFF, 0x02, 0xFF, 0xFF, 0x02, 0x02, 0xFF, 0x02, 0x03, 0xFE, 0x01, 0x07, 0xFF, 0x02, 0xFF, 0xFF, 0xFE, 0xFF},
    {0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFE, 0xFE, 0xFF, 0xFE, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0x04, 0x06, 0xFF},
    {0x04, 0x06, 0x05, 0xFE, 0x06, 0xFE, 0x06, 0x06, 0x06, 0x04, 0x06, 0x05, 0xFE, 0xFE, 0x06, 0x04, 0x06, 0x05, 0x04, 0x06, 0x05, 0xFE, 0x06, 0xFE, 0x04, 0x06, 0x05, 0xFE, 0xFE, 0x06}};

byte col, row, nb = 0, bc = 0; // general
byte bb[8];                    // byte buffer for reading from PROGMEM

// #include "RTClib.h"
// RTC_Millis RTC;
byte hr, mn, se, osec;
long firstDigit, secondDigit, thirdDigit;

// VARIABLES:
// Rotary's encoder middle pin should be connected to a ground
// flip left and right pins to change rotation directions to modify the timer delay
const int rotaryEncoderPinOne = 3; // left pin (A)
const int rotaryEncoderPinTwo = 2; // right pin (B)

const int rotaryEncoderSpeedLimit = 2; // the rotary encoder speed threshold above which the increment value is multiplied by the current speed value. This is to avoid the encoder to be too sensitive when setting the timer delay.
const int encoderButtonPin = 4;        // rotary encoder's push button connection pin (resets timer to 0)

int increment = 100;          // change this value to change the milliseconds increment when setting the timer delay
const int lcdOffset = 3;      // sets the display position for the very left BIG digit (we use only three digits for now)
const int timerButtonPin = 6; // timer start push button connection pin
const int relayOnePin = 7;    //  the pin number of the relay controlling the enlarger lamp
const int manualLightPin = 8; // manual light indicator pin
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;           // the last time the output pin was toggled (debounce)
unsigned long debounceDelay = 50;             // the debounce time; increase if the output flickers (50ms is a good value)
unsigned long turnEnlargerLampOnDelay = 2000; // turn the lamp ON after the set button is pressed atleast for 2 seconds (2000ms)
const long maxTimerDelay = 99000;             // maximum timer delay is 99 seconds (99000ms)
const unsigned long duration = 100000;        // timer increment in microseconds (100000us = 100ms)

long timerDelay = 0;
long storedTimerDelay = 0;
char tempString[26];
int tempIncrement = 0;

int encoderButtonState;           // the current reading from the input pin (encoder's push button)
int lastEncoderButtonState = LOW; // the previous reading from the input pin (encoder's push button)
int timerButtonState;             // the current reading from the input pin (timer's push button)
int lastTimerButtonState = LOW;   // the previous reading from the input pin (timer's push button)

volatile boolean startExposure = false;
volatile boolean turnOnEnlargerLamp = false;         // A flag indicating whether the enlarger lamp should be turned on or off. Stores an enlarger's lamp state in timer mode (Enlarger is turned on/off automatically).
volatile boolean turnManuallyOnEnlargerLamp = false; // A flag indicating whether the enlarger lamp is manually turned on or off. Stores an enlarger's lamp state in manual mode (Enlarger is turned on/off manually).
volatile boolean timerButtonIsPressed = false;       // stores a timer button state (pressed or not pressed)

unsigned long _micro, time = micros();

int eeAddress = 0; // A EEPROM memory location to store the exposure time between power cycles. This is used to restore the exposure time after the power cycle.
// set up encoder object
MD_REncoder rotaryEncoder = MD_REncoder(rotaryEncoderPinOne, rotaryEncoderPinTwo);
// set up LCD display object
// Pins for the LCD are SCL A5, SDA A4 for Arduino UNO (check your hardware specs to see which pins are I2C bus)
LiquidCrystal_I2C lcd(I2C_ADDR, EN_pin, RW_pin, RS_pin, D4_pin, D5_pin, D6_pin, D7_pin, BACK_pin, POSITIVE);

//*****************************************************************************************//
//                                      Initial Setup
//*****************************************************************************************//
void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // Waiting for Serial Monitor
  randomSeed(analogRead(0));
  //  RTC.begin(DateTime(__DATE__, __TIME__));
  lcd.begin(20, 4);
  // ------- Quick 3 blinks of an LCD's backlight  -------------
  for (int i = 0; i < 3; i++)
  {
    lcd.backlight();
    delay(250);
    lcd.noBacklight();
    delay(250);
  }
  lcd.backlight(); // finish with backlight on

  for (nb = 0; nb < 7; nb++)
  { // create 8 custom characters
    for (bc = 0; bc < 8; bc++)
      bb[bc] = pgm_read_byte(&custom[nb][bc]);
    lcd.createChar(nb + 1, bb);
  }
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print(F("DARKROOM"));
  lcd.setCursor(3, 1);
  lcd.print(F("EXPOSURE TIMER"));
  lcd.setCursor(6, 3);
  lcd.print(F("V"));
  lcd.print(build);
  lcd.print(F("."));
  lcd.print(revision);
  lcd.print(F(" "));
  lcd.print(freeRam());
  lcd.print(F("B"));
  delay(1500);
  lcd.clear();

  showInitScreen();
  printNum(0, lcdOffset + 8);

  // read stored exposure time
  EEPROM.get(eeAddress, timerDelay);

  rotaryEncoder.begin();
  pinMode(encoderButtonPin, INPUT_PULLUP);
  pinMode(timerButtonPin, INPUT_PULLUP);

  // lamp test
  pinMode(relayOnePin, OUTPUT);
  digitalWrite(relayOnePin, HIGH);
  delay(1000);
  digitalWrite(relayOnePin, LOW);

  pinMode(manualLightPin, OUTPUT);
}

/**
 *                                      MAIN LOOP
 */

void loop()
{
  // handle input from buttons and rotary encoder
  inputHandler();

  // if exposure is not in progress, check for changes to timer delay
  if (!startExposure)
  {
    readEncoder();
  }
  // if exposure is in progress, activate relay and check if exposure is complete
  else
  {
    startRelay();
  }

  // calculate number of seconds remaining
  se = timerDelay / 100; // seconds

  // calculate digits for timer display
  thirdDigit = floor(timerDelay / 10000);                                                 // 10000ms = 10s
  secondDigit = floor((timerDelay - thirdDigit * 10000) / 1000);                          // 1000ms = 1s
  firstDigit = floor((timerDelay - thirdDigit * 10000 - secondDigit * 1000) / increment); // 100ms = 0.1s

  // update timer display if seconds remaining have changed
  if (se != osec)
  {
    // third digit
    if (thirdDigit != 0)
    {
      printNum(thirdDigit, lcdOffset);
    }
    else
    {
      eraseNum(lcdOffset);
    }

    // second digit
    if (secondDigit != 0 || thirdDigit != 0)
    {
      printNum(secondDigit, lcdOffset + 4);
    }
    else
    {
      eraseNum(lcdOffset + 4);
    }

    // first digit
    printNum(firstDigit, lcdOffset + 8);

    // show initial screen
    showInitScreen();

    // update previous seconds value
    osec = se;
  }
}

/**
 *                                      SUBROUTINES
 */

/**
 * @brief Prints a digit on an LCD screen.
 *
 * @param digit A digit to print.
 * @param leftAdjust A left adjustment of a digit.
 */
void printNum(byte digit, byte leftAdjust)
{
  for (byte row = 0; row < 4; row++)
  {
    lcd.setCursor(leftAdjust, row);
    for (byte col = digit * 3; col < digit * 3 + 3; col++)
    {
      lcd.write(pgm_read_byte(&bn[row][col]));
    }
  }
}

/**
 * @brief Erases a digit from an LCD screen.
 *
 * @param leftAdjust A left adjustment of a digit.
 */
void eraseNum(byte leftAdjust)
{
  for (byte row = 0; row < 4; row++)
  {
    lcd.setCursor(leftAdjust, row);
    for (byte col = leftAdjust; col < leftAdjust + 3; col++)
    {
      lcd.print(F(" "));
    }
  }
}

/**
 * @brief Prints a colon on an LCD screen.
 *
 * @param leftAdjust A left adjustment of a colon.
 */
void printColon(byte leftAdjust)
{
  for (byte row = 0; row < 4; row++)
  {
    lcd.setCursor(leftAdjust, row);
    if (row == 1 || row == 2)
      lcd.print(F("."));
    else
      lcd.print(F(" "));
  }
}

/**
 * @brief Prints a dot on an LCD screen.
 *
 * @param leftAdjust A left adjustment of a dot.
 */
void printDot(byte leftAdjust)
{
  for (byte row = 0; row < 4; row++)
  {
    lcd.setCursor(leftAdjust, row);
    if (row == 3)
      lcd.print(F("."));
    else
      lcd.print(F(" "));
  }
}

/**
 * @brief Reads a rotary encoder and updates the timer delay according to its rotation direction and speed.
 */
void readEncoder()
{
  uint8_t x = rotaryEncoder.read();
  uint32_t tempIncrement = increment;

  if (x)
  {
    // enables rotary encoder value changed depending on it's rotation speed.
    if (ENABLE_SPEED && rotaryEncoder.speed() > rotaryEncoderSpeedLimit)
    {
      tempIncrement = tempIncrement * rotaryEncoder.speed();
    }
    if (x == DIR_CCW)
    {
      // if Encoder is moved counter clock wise (CCW, i.e. backwards), decrease timer delay by defined increment value
      DEBUG_PRINT("CCW ");
      timerDelay = timerDelay > tempIncrement ? timerDelay - tempIncrement : 0;
    }
    else if (x != DIR_NONE)
    {
      // if Encoder is moved clock wise (CW, i.e. forward), increase timer delay by defined increment value
      DEBUG_PRINT("CW ");
      timerDelay = timerDelay + tempIncrement < maxTimerDelay ? timerDelay + tempIncrement : maxTimerDelay;
    }
    DEBUG_PRINT("timerDelay: ");
    DEBUG_PRINT(timerDelay);
  }
}
/**
 *                                      OPERATION ROUTINES
 */

/**
 * @brief Returns the number of bytes currently free in RAM.
 *
 * @return the number of free bytes in RAM
 */
int freeRam()
{
  extern char __bss_end;
  extern char *__brkval;

  int free_memory;
  if (reinterpret_cast<int>(__brkval) == 0)
  {
    // If heap is not being used, free memory is the space between the end of the static data
    // and the start of the stack
    free_memory = reinterpret_cast<int>(&free_memory) - reinterpret_cast<int>(&__bss_end);
  }
  else
  {
    // If heap is being used, free memory is the space between the end of the heap and the start of the stack
    free_memory = reinterpret_cast<int>(&free_memory) - reinterpret_cast<int>(__brkval);
  }

  return free_memory;
}

/**
 *                                      UTILITY ROUTINES
 */

/**
 *  @brief Turns the enlarger lamp on if it is not manually turned off.
 */
void turnEnlargerLampOn()
{
  if (!turnManuallyOnEnlargerLamp)
  {
    turnEnlargerLampOff();
  }
  if (turnOnEnlargerLamp)
  {
    DEBUG_PRINT("Turning an enlarger's lamp ON\n");
    digitalWrite(relayOnePin, HIGH);
    lcd.noBacklight();
    turnOnEnlargerLamp = false;
  }
}

/**
 * @brief Starts the exposure timer.
 */

void startRelay()
{
  turnEnlargerLampOn();
  if ((_micro = micros()) - time > duration)
  {
    // check to see if micros() has rolled over, if not,
    // then increment "time" by duration
    _micro < time ? time = _micro : time += duration;

    timerDelay = timerDelay - increment;
    if (timerDelay <= 0)
    {
      timerDelay = storedTimerDelay;
      turnEnlargerLampOff();
    }
  }
}

/**
 * @brief Turns the enlarger lamp off.
 */

void turnEnlargerLampOff()
{
  DEBUG_PRINT("Exposure Timer Reset.");
  DEBUG_PRINT("Turning an enlarger's lamp OFF\n");
  startExposure = false;
  turnOnEnlargerLamp = false;
  turnManuallyOnEnlargerLamp = false;
  timerButtonIsPressed = false;
  digitalWrite(relayOnePin, LOW);
  digitalWrite(manualLightPin, LOW);
  lcd.backlight();
}

/*
 * @brief Shows the initialization screen.
 */

void showInitScreen()
{
  printDot(lcdOffset + 7);
  lcd.setCursor(lcdOffset + 12, 3);
  lcd.print(F("SEC"));
  Serial.print("\nDarkroom Timer is initialized. Available memory is: ");
  Serial.print(freeRam());
  Serial.println(" bytes");
}

/**
 * @brief Handles the input from the rotary encoder and the timer button.
 */

void inputHandler()
{

  int encoderButtonPinValue = digitalRead(encoderButtonPin);
  int timerButtonPinValue = digitalRead(timerButtonPin);

  // If the switch changed, due to noise or pressing:
  if (encoderButtonPinValue != lastEncoderButtonState || timerButtonPinValue != lastTimerButtonState)
  {
    // reset the debouncing timer
    if (!timerButtonIsPressed)
    {
      lastDebounceTime = millis();
    }
  }

  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    // whatever the reading is at, it's been there longer than the debounce
    // delay, so take it as the actual current state:

    // if the encoder button state has changed:
    if (encoderButtonPinValue != encoderButtonState)
    {
      encoderButtonState = encoderButtonPinValue;
      if (encoderButtonState == LOW)
      {
        timerDelay = 0;
        turnEnlargerLampOff();
      }
    }

    if (timerButtonPinValue != timerButtonState)
    {
      timerButtonState = timerButtonPinValue;
      if (timerButtonState == LOW)
      {
        lastDebounceTime = millis();
        timerButtonIsPressed = true;
      }
    }
    if (timerButtonIsPressed)
    {
      if (timerButtonState == HIGH)
      {
        storedTimerDelay = timerDelay;
        EEPROM.put(eeAddress, storedTimerDelay);
        turnOnEnlargerLamp = true; // signals to turn on the lamp
        time = micros();           // hwd added so timer will reset if stopped and then started
        timerButtonIsPressed = false;

        if (((millis() - lastDebounceTime) < turnEnlargerLampOnDelay))
        {
          DEBUG_PRINT("Timer Button is Released.");
          DEBUG_PRINT("Staring Exposure.");
          startExposure = true; // RELAY
          turnManuallyOnEnlargerLamp = true;
        }
        else
        {
          startExposure = false;
          DEBUG_PRINT("Timer Button is Released after a manual delay.");
          DEBUG_PRINT((millis() - lastDebounceTime));
          //      lastDebounceTime = millis();
          //      timerButtonIsPressed = true;
          turnManuallyOnEnlargerLamp = !turnManuallyOnEnlargerLamp;
          DEBUG_PRINT("turnManuallyOnEnlargerLamp : ");
          DEBUG_PRINT(turnManuallyOnEnlargerLamp);
          digitalWrite(manualLightPin, LOW);
          turnEnlargerLampOn();
        }
      }
      else if (((millis() - lastDebounceTime) > turnEnlargerLampOnDelay))
      {
        DEBUG_PRINT("Timer Button is Pressed longer than a manual delay.");
        digitalWrite(manualLightPin, HIGH);
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastEncoderButtonState = encoderButtonPinValue;
  lastTimerButtonState = timerButtonPinValue;
}
