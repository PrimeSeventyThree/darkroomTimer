/*
 * File: darkroom_timer.ino
 * Project: Drakroom Timer
 * File Created: Wednesday, 21st July 2021 10:40:30 am
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Saturday, 12th November 2022 3:00:11 pm
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
//    REQUREMENTS:
//    MD_REncoder - Rotary Encoder Library (https://github.com/MajicDesigns/MD_REncoder)
//    New-LiquidCrystal - LCD Library for Arduino and Chipkit (https://github.com/fmalpartida/New-LiquidCrystal)
//********************************************************************************

#define build 1
#define revision 6
//********************************************************************************

#define DEBUG           // Comment out this line to disable all debug output before uploading final sketch to a board
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
const int rotaryEncoderPinOne = 2; // left pin
const int rotaryEncoderPinTwo = 3; // right pin

const int encoderButtonPin = 4; // rotary encoder's push button connection pin (resets timer to 0)

int increment = 100;          // change this value to change the milliseconds increment when setting the timer
const int lcdOffset = 3;      // sets the display position for the very left BIG digit (we use only three digits for now)
const int timerButtonPin = 6; // timer start push button connectionpin
const int relayOnePin = 7;    // relay number One control pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;           // the last time the output pin was toggled
unsigned long debounceDelay = 50;             // the debounce time; increase if the output flickers
unsigned long toggleEnlargerLampDelay = 2000; // turn the lamp ON after the set button is pressed atleast for 2 seconds
const long maxTimerDelay = 99000;             // maximum timer delay is 99 seconds
const unsigned long duration = 100000;        // timer increment in microseconds

long timerDelay = 0;
long storedTimerDelay = 0;
char tempString[26];
int tempIncrement = 0;

int encoderButtonState;           // the current reading from the input pin
int lastEncoderButtonState = LOW; // the previous reading from the input pin
int timerButtonState;             // the current reading from the input pin
int lastTimerButtonState = LOW;   // the previous reading from the input pin

volatile boolean startExposure = false;
volatile boolean lampIsOn = false;             // stores an enlarger's lamp state
volatile boolean isEnlargerLampOn = false;     // stores an enlarger's lamp state in manual mode (Enlarger is turned on/off manually)
volatile boolean timerButtonIsPressed = false; // stores a timer button state

unsigned long _micro, time = micros();

int eeAddress = 0; // A EEPROM memory location to store the exposure time between power cycles.
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
  //    printNum(random(0,10),0);
  //    printNum(random(0,10),17);
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
}

//*****************************************************************************************//
//                                      MAIN LOOP
//*****************************************************************************************//
void loop()
{

  inputHandler();

  if (!startExposure)
  {
    readEncoder();
  }
  else
  {
    startRelay();
  }

  //    DateTime now = RTC.now();
  //    hr = now.hour();
  se = timerDelay / 100;

  thirdDigit = floor(timerDelay / 10000);
  secondDigit = floor((timerDelay - thirdDigit * 10000) / 1000);
  firstDigit = floor((timerDelay - thirdDigit * 10000 - secondDigit * 1000) / increment);

  if (se != osec)
  {
    // DEBUG_PRINT(thirdDigit);
    // DEBUG_PRINT(secondDigit);
    // DEBUG_PRINT(firstDigit);

    if (thirdDigit != 0)
    {
      printNum(thirdDigit, lcdOffset);
      //      if (secondDigit != 0) {
      printNum(secondDigit, lcdOffset + 4);
      printNum(firstDigit, lcdOffset + 8);
    }
    else
    {
      eraseNum(lcdOffset);

      if (secondDigit != 0)
      {
        printNum(secondDigit, lcdOffset + 4);
        printNum(firstDigit, lcdOffset + 8);
      }
      else
      {
        eraseNum(lcdOffset + 4);
        printNum(firstDigit, lcdOffset + 8);
      }
    }
    showInitScreen();
    osec = se;
  }

  //      delay(50);                // not strictly necessary
}
// ********************************************************************************** //
//                                      SUBROUTINES
// ********************************************************************************** //
void printNum(byte digit, byte leftAdjust)
{
  for (row = 0; row < 4; row++)
  {
    lcd.setCursor(leftAdjust, row);
    for (col = digit * 3; col < digit * 3 + 3; col++)
    {
      lcd.write(pgm_read_byte(&bn[row][col]));
    }
  }
}

void eraseNum(byte leftAdjust)
{
  for (row = 0; row < 4; row++)
  {
    lcd.setCursor(leftAdjust, row);
    for (col = leftAdjust; col < leftAdjust + 3; col++)
    {
      lcd.print(F(" "));
    }
  }
}

void printColon(byte leftAdjust)
{
  for (row = 0; row < 4; row++)
  {
    lcd.setCursor(leftAdjust, row);
    if (row == 1 || row == 2)
      lcd.print(F("."));
    else
      lcd.print(F(" "));
  }
}

void printDot(byte leftAdjust)
{
  for (row = 0; row < 4; row++)
  {
    lcd.setCursor(leftAdjust, row);
    if (row == 3)
      lcd.print(F("."));
    else
      lcd.print(F(" "));
  }
}

void readEncoder()
{
  // seconds = constrain (seconds, -1, 60); //constrains the seconds value between -1 and 60
  uint8_t x = rotaryEncoder.read();
  tempIncrement = increment;
  if (x)
  {
#if ENABLE_SPEED
    if (rotaryEncoder.speed() > 4)
    {
      tempIncrement = tempIncrement * rotaryEncoder.speed();
    }
#endif
    if (x == DIR_CCW)
    {
      // if Encoder is moved counter clock wise (CCW, i.e. backwards), decrease timer delay by defined increment value
      DEBUG_PRINT("CCW ");
      timerDelay = timerDelay - tempIncrement;
      if (timerDelay < 0) // timer delay can't be less than 0 seconds
      {
        timerDelay = 0;
      }
    }
    else if (x != DIR_NONE)
    {
      // if Encoder is moved clock wise (CW, i.e. forward), increase timer delay by defined increment value
      DEBUG_PRINT("CW ");
      timerDelay = timerDelay + tempIncrement;
      if (timerDelay >= maxTimerDelay) // timer delay can't be more than maxTimerDelay (see it's definition above) seconds
      {
        timerDelay = maxTimerDelay;
      }
    }

    DEBUG_PRINT("timerDelay: ");
    DEBUG_PRINT(timerDelay);
  }
}
// ********************************************************************************** //
//                                      OPERATION ROUTINES
// ********************************************************************************** //
// FREERAM: Returns the number of bytes currently free in RAM
int freeRam(void)
{
  extern int __bss_end, *__brkval;
  int free_memory;
  if ((int)__brkval == 0)
  {
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  }
  else
  {
    free_memory = ((int)&free_memory) - ((int)__brkval);
  }
  return free_memory;
}

// ********************************************************************************** //
//                                      UTILITY ROUTINES
// ********************************************************************************** //

// toggleEnlargerLamp()

void toggleEnlargerLamp()
{
  // pinMode(relayOnePin, OUTPUT);
  if (lampIsOn)
  {
    DEBUG_PRINT("Turning an enlander's lamp ON\n");
    digitalWrite(relayOnePin, HIGH);
    lcd.noBacklight();
    lampIsOn = false;
    // isEnlargerLampOn = false;
  }
}

// startRelay

void startRelay()
{
  toggleEnlargerLamp();
  if ((_micro = micros()) - time > duration)
  {
    // check to see if micros() has rolled over, if not,
    // then increment "time" by duration
    _micro < time ? time = _micro : time += duration;

    timerDelay = timerDelay - increment;
    if (timerDelay <= 0)
    {
      timerDelay = storedTimerDelay;
      resetTimer();
    }
  }
}

// resetTimer

void resetTimer()
{
  DEBUG_PRINT("Exposure Timer Reset.");
  DEBUG_PRINT("Turning an enlander's lamp OFF\n");
  startExposure = false;
  lampIsOn = false;
  timerButtonIsPressed = false;
  digitalWrite(relayOnePin, LOW);
  lcd.backlight();
}

// showInitScreen

void showInitScreen()
{
  printDot(lcdOffset + 7);
  lcd.setCursor(lcdOffset + 12, 3);
  lcd.print(F("SEC"));
  Serial.print("\nDarkroom Timer is initialized. Available memory is: ");
  Serial.print(freeRam());
  Serial.println(" bytes");
}

// main input handler

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
        resetTimer();
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

    if (timerButtonIsPressed && timerButtonState == HIGH)
    {
      storedTimerDelay = timerDelay;
      EEPROM.put(eeAddress, storedTimerDelay);
      lampIsOn = true; // signals to turn on the lamp
      // isEnlargerLampOn = false;
      time = micros(); // hwd added so timer will reset if stopped and then started
      timerButtonIsPressed = false;

      if (((millis() - lastDebounceTime) < toggleEnlargerLampDelay))
      {
        DEBUG_PRINT("Timer Button is Released.");
        DEBUG_PRINT("Staring Exposure.");
        startExposure = true; // RELAY
        // isEnlargerLampOn = false;
      }
      else
      {
        startExposure = false;
        DEBUG_PRINT("Timer Button is Pressed.");
        DEBUG_PRINT((millis() - lastDebounceTime));
        //      lastDebounceTime = millis();
        //      timerButtonIsPressed = true;
        // isEnlargerLampOn = false;
        toggleEnlargerLamp();
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastEncoderButtonState = encoderButtonPinValue;
  lastTimerButtonState = timerButtonPinValue;
}
