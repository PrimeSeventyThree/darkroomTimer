//**************************************************************************************************
//                                 DARKROOM EXPOSURE TIMER
//                                 Andrei Grichine, December 2017
//
//                                 this code is based on:
//                                 BIG FONT (4-line) LCD CHARACTERS
//                                 http://woodsgood.ca/projects/2015/02/27/4-line-lcd-big-numbers/
//                                 Adrian Jones, February 2015
//
//**************************************************************************************************

#define build 1
#define revision 2
//********************************************************************************************

#include <MD_REncoder.h>             // Rotary Encoder

#include <avr/pgmspace.h>            // for memory storage in program space

#include <Wire.h>
#include <LCD.h>                     // Standard lcd library
#include <LiquidCrystal_I2C.h>       // library for I@C interface

#define I2C_ADDR  0x27               // address found from I2C scanner
#define RS_pin    0                  // pin configuration for LCM1602 interface module
#define RW_pin    1
#define EN_pin    2
#define BACK_pin  3
#define D4_pin    4
#define D5_pin    5
#define D6_pin    6
#define D7_pin    7

#define  LED_OFF  0
#define  LED_ON  1

LiquidCrystal_I2C lcd(I2C_ADDR, EN_pin, RW_pin, RS_pin, D4_pin, D5_pin, D6_pin, D7_pin, BACK_pin, POSITIVE);
//Pins for the LCD are SCL A5, SDA A4

const char custom[][8] PROGMEM = {
  {0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F},      // char 1: bottom right triangle
  {0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F},      // char 2: bottom block
  {0x00, 0x00, 0x00, 0x00, 0x10, 0x1C, 0x1E, 0x1F},      // char 3: bottom left triangle
  {0x1F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00},      // char 4: top right triangle
  {0x1F, 0x1E, 0x1C, 0x10, 0x00, 0x00, 0x00, 0x00},      // char 5: top left triangle
  {0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00},      // char 6: upper block
  {0x1F, 0x1F, 0x1E, 0x1C, 0x18, 0x18, 0x10, 0x10}       // char 7: full top right triangle
  // room for another one!
};

const char bn[][30] PROGMEM = {                             // organized by row
  //         0               1               2               3               4              5               6                7               8               9
  {0x01, 0x02, 0x03, 0x01, 0x02, 0xFE, 0x01, 0x02, 0x03, 0x01, 0x02, 0x03, 0x02, 0xFE, 0x02, 0x02, 0x02, 0x02, 0x01, 0x02, 0x03, 0x02, 0x02, 0x02, 0x01, 0x02, 0x03, 0x01, 0x02, 0x03},
  {0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0x01, 0x02, 0xFF, 0xFE, 0x02, 0xFF, 0xFF, 0x02, 0xFF, 0xFF, 0x02, 0x02, 0xFF, 0x02, 0x03, 0xFE, 0x01, 0x07, 0xFF, 0x02, 0xFF, 0xFF, 0xFE, 0xFF},
  {0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFE, 0xFE, 0xFF, 0xFE, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0x04, 0x06, 0xFF},
  {0x04, 0x06, 0x05, 0xFE, 0x06, 0xFE, 0x06, 0x06, 0x06, 0x04, 0x06, 0x05, 0xFE, 0xFE, 0x06, 0x04, 0x06, 0x05, 0x04, 0x06, 0x05, 0xFE, 0x06, 0xFE, 0x04, 0x06, 0x05, 0xFE, 0xFE, 0x06}
};

byte col, row, nb = 0, bc = 0;                            // general
byte bb[8];                                               // byte buffer for reading from PROGMEM

//#include "RTClib.h"
//RTC_Millis RTC;
byte hr, mn, se, osec;
long firstDigit, secondDigit, thirdDigit;

// set up encoder object
MD_REncoder R = MD_REncoder(2, 3);


// VARIABLES:
int increment = 100; //change this value to change the milliseconds increment when setting the timer
const int lcdOffset = 3; //sets the display position for the very left BIG digit (we use only three digits for now)
const int encoderButtonPin = 4;   // the number of the pushbutton pin
const int timerButtonPin = 6;     // the number of the pushbutton pin
const int relayOnePin = 7;        // relay number One control pin
const int relayTwoPin = 8;        // relay number Two control pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

const unsigned long maxTimerDelay = 99000; //maximum timer delay is 99 seconds
long timerDelay = 0;
char tempString[100];
int tempIncrement = 0;

int encoderButtonState;             // the current reading from the input pin
int lastEncoderButtonState = LOW;   // the previous reading from the input pin
int timerButtonState;             // the current reading from the input pin
int lastTimerButtonState = LOW;   // the previous reading from the input pin

boolean startExposure = false;
boolean lampIsOn = false; //stores an enlarger's lamp state
//*****************************************************************************************//
//                                      Initial Setup
//*****************************************************************************************//
void setup() {
  Serial.begin(57600);
  randomSeed(analogRead(0));
  //  RTC.begin(DateTime(__DATE__, __TIME__));
  lcd.begin(20, 4);
  // ------- Quick 3 blinks of backlight  -------------
  for (int i = 0; i < 3; i++)
  {
    lcd.backlight();
    delay(250);
    lcd.noBacklight();
    delay(250);
  }
  lcd.backlight(); // finish with backlight on

  //  // Switch on the backlight
  //  lcd.setBacklightPin(BACK_pin, POSITIVE);
  //  lcd.setBacklight(128);

  for (nb = 0; nb < 7; nb++ ) {                 // create 8 custom characters
    for (bc = 0; bc < 8; bc++) bb[bc] = pgm_read_byte( &custom[nb][bc] );
    lcd.createChar ( nb + 1, bb );
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

  R.begin();

  pinMode(encoderButtonPin, INPUT_PULLUP);
  pinMode(timerButtonPin, INPUT_PULLUP);

  //lamp test
  pinMode(relayOnePin, OUTPUT);
  digitalWrite(relayOnePin, HIGH);
  delay(1000);
  digitalWrite(relayOnePin, LOW);
}

//*****************************************************************************************//
//                                      MAIN LOOP
//*****************************************************************************************//
void loop() {

  if (!startExposure) {
    readEncoder();
  } else {
    startRelay();
  }
  //    DateTime now = RTC.now();
  //    hr = now.hour();
  se = timerDelay / 100;

  thirdDigit = floor(timerDelay / 10000);
  secondDigit = floor((timerDelay - thirdDigit * 10000) / 1000);
  firstDigit = floor((timerDelay - thirdDigit * 10000 - secondDigit * 1000) / increment);

  if (se != osec) {

    sprintf(tempString,"digits to display: ", "%llu, %llu, %llu", se,osec,firstDigit);
    Serial.println(tempString);

    if (thirdDigit != 0) {
      printNum(thirdDigit, lcdOffset);
      //      if (secondDigit != 0) {
      printNum(secondDigit, lcdOffset + 4);
      printNum(firstDigit, lcdOffset + 8);
    } else {
      eraseNum(lcdOffset);

      if (secondDigit != 0) {
        printNum(secondDigit, lcdOffset + 4);
        printNum(firstDigit, lcdOffset + 8);
      } else {
        eraseNum(lcdOffset + 4);
        printNum(firstDigit, lcdOffset + 8);
      }
    }
    showInitScreen();
    osec = se;
  }

  inputHandler();
  //      delay(50);                // not strictly necessary

}
// ********************************************************************************** //
//                                      SUBROUTINES
// ********************************************************************************** //
void printNum(byte digit, byte leftAdjust) {
  for (row = 0; row < 4; row++) {
    lcd.setCursor(leftAdjust, row);
    for (col = digit * 3; col < digit * 3 + 3; col++) {
      lcd.write(pgm_read_byte( &bn[row][col]) );
    }
  }
}

void eraseNum(byte leftAdjust) {
  for (row = 0; row < 4; row++) {
    lcd.setCursor(leftAdjust, row);
    for (col = leftAdjust; col < leftAdjust + 3; col++) {
      lcd.print(F(" "));
    }
  }
}

void printColon(byte leftAdjust) {
  for (row = 0; row < 4; row++) {
    lcd.setCursor(leftAdjust, row);
    if (row == 1 || row == 2) lcd.print(F(".")); else lcd.print(F(" "));
  }
}

void printDot(byte leftAdjust) {
  for (row = 0; row < 4; row++) {
    lcd.setCursor(leftAdjust, row);
    if (row == 3) lcd.print(F(".")); else lcd.print(F(" "));
  }
}

void readEncoder () {
  // seconds = constrain (seconds, -1, 60); //constrains the seconds value between -1 and 60
  uint8_t x = R.read();
  tempIncrement = increment;
  if (x)
  {
#if ENABLE_SPEED
    if (R.speed() > 4) {
      tempIncrement = tempIncrement * R.speed();
    }
#endif
    if (x == DIR_CW) {//if Encoder is moved forwards (CW), advance seconds by defined increment value
      Serial.print("CW ");

      timerDelay = timerDelay + tempIncrement;
    } else if (x != DIR_NONE) {
      Serial.print("CCW ");
      timerDelay = timerDelay - tempIncrement;//if seconds were not = 0, then decrease seconds value by the increment value
      if (timerDelay < 0) {
        timerDelay = 0;
      }
    }
    if (timerDelay >= maxTimerDelay) {
      timerDelay = maxTimerDelay;
    }

    Serial.print("timerDelay: ");
    Serial.print(timerDelay);
    Serial.print("\n");
  }
}
// ********************************************************************************** //
//                                      OPERATION ROUTINES
// ********************************************************************************** //
// FREERAM: Returns the number of bytes currently free in RAM
int freeRam(void) {
  extern int  __bss_end, *__brkval;
  int free_memory;
  if ((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
  }
  return free_memory;
}

// ********************************************************************************** //
//                                      UNILITY ROUTINES
// ********************************************************************************** //
// startRelay

void startRelay() {

  if (lampIsOn) {
    Serial.print("turn ON an enlander's lamp\n");
    digitalWrite(relayOnePin, HIGH);
    lampIsOn = false;
    lcd.noBacklight();
  }
  timerDelay = timerDelay - increment;
  delay(increment);
  if (timerDelay <= 0) {
    Serial.print("turn OFF an enlander's lamp\n");
    startExposure = false;
    lampIsOn = false;
    timerDelay = 0;
    digitalWrite(relayOnePin, LOW);
    lcd.backlight();
  }
}

void resetTimer() {
  Serial.print("Exposure Timer Reset. \n");
  startExposure = false;
  lampIsOn = false;
  timerDelay = 0;
}


void showInitScreen() {
  printDot(lcdOffset + 7);
  lcd.setCursor(lcdOffset + 12, 3);
  lcd.print(F("SEC"));
}

void inputHandler() {

  int encoderButtonPinValue = digitalRead(encoderButtonPin);
  //                  Serial.print("\nEncoder Button  value ");
  //                  Serial.print(encoderButtonPinValue);
  //                  Serial.print("\n");

  int timerButtonPinValue = digitalRead(timerButtonPin);
  //  Serial.print("\Timer Button  value ");
  //  Serial.print(timerButtonPinValue);
  //  Serial.print("\n");

  // If the switch changed, due to noise or pressing:
  if (encoderButtonPinValue != lastEncoderButtonState || timerButtonPinValue != lastTimerButtonState) {
    // reset the debouncing timer
    //    Serial.print("reset the debouncing timer\n");
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    //             Serial.print("\nEncoder Button changed state\n");

    // if the encoder button state has changed:
    if (encoderButtonPinValue != encoderButtonState) {
      encoderButtonState = encoderButtonPinValue;
      //              Serial.print("\nEncoder Button changed state\n");

      if (encoderButtonState == LOW) {
        resetTimer();
      }
    }

    // if the timer button state has changed:
    if (timerButtonPinValue != timerButtonState) {
      timerButtonState = timerButtonPinValue;
      if (timerButtonState == LOW) {
        Serial.print("\nTimer Button is Pressed\n");
        startExposure = true; //RELAY
        lampIsOn = true; // signals to turn on the lamp
      }

    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastEncoderButtonState = encoderButtonPinValue;
  lastTimerButtonState = timerButtonPinValue;
}

