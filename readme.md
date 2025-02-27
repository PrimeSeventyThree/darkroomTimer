# Darkroom Timer for Photographic Development

![darkroom timer](./images/darkroom-timer-enclosure.jpg)

This Arduino-based project serves as a reliable timer for the photographic darkroom process. It uses a custom LCD character display for clear and precise visual feedback, making it an essential tool for photographers during the development of their work. It also implements EEPROM wear leveling to extend the lifespan of the memory.

## Features

- Customizable timer settings with EEPROM for persistent storage, including wear leveling to maximize EEPROM lifespan.
- Intuitive interface with a rotary encoder for adjusting the timer delay.
- Start/stop button for exposure control with a long-press feature to manually control the enlarger lamp.
- Automatic reset to zero with the rotary encoder's push button.
- Relay output to control the enlarger lamp.
- EEPROM failure detection and warning.
- Splash screen displaying version information and last stored delay.

## Long-Press Functionality

The exposure button has an additional long-press feature:

- A long press (at least 2 seconds) on the exposure button will turn the enlarger lamp ON, bypassing the timer.
- Pressing the button again will turn the lamp OFF.
- The long-press delay is defined by `turnEnlargerLampOnDelay = 2000;` which corresponds to 2000 milliseconds (2 seconds).  This value can be adjusted in `src/constants.h`.

## Maximum Timer Delay

The maximum timer delay that can be set is 599 seconds (599000 milliseconds). This limit ensures that the exposure times are kept within a practical range for darkroom processes and prevents potential overflow issues.

## EEPROM Wear Leveling

This project incorporates EEPROM wear leveling to extend the life of the EEPROM. The timer values are not written into a single memory location.
- The EEPROM wears evenly by distributing writes across multiple memory locations.
- The EEPROM start and end address can be configured within `src/constants.h`.
- An EEPROM address pointer keeps track of where in memory the last EEPROM write happened.
- EEPROM writes are skipped if the value has not changed.
- EEPROM is initialized with EEPROM_INIT_VALUE if it has not been configured previously.

## Requirements

### Software Libraries
- **MD_REncoder**: Rotary Encoder Library. Installable from [MD_REncoder GitHub Repository](https://github.com/MajicDesigns/MD_REncoder).
- **New-LiquidCrystal**: LCD Library for Arduino and Chipkit. Installation details can be found at [New-LiquidCrystal GitHub Repository](https://github.com/fmalpartida/New-LiquidCrystal).

### Hardware Components
- A rotary encoder with a built-in push button to adjust the timer delay and reset the timer.
- A push button to start the exposure, with a long-press feature to manually control the enlarger lamp.
- A relay module to switch the enlarger lamp on and off.
- LCM1602 interface module with a 4x20 character LCD display.
- Arduino compatible board (e.g. Arduino Uno, Nano, Mega).

### Wiring
- **Rotary Encoder**: Middle pin to ground, A and B pins to digital inputs. Push button to reset the timer.  See `src/encoderHandler.h` for pin definitions.
- **Push Button**: Digital input to start the exposure, with a long-press functionality.  See `src/ButtonHandler.h` for pin definitions.
- **Relay**: Digital output to control the enlarger lamp. See `src/LampControl.h` for the pin definition.
- **Manual Light Indicator**: Digital output to indicate manual light mode. See `src/LampControl.h` for the pin definition.

## LCD I2C connection

Use the I2C interface module (for example LCM1602) to connect the LCD to the Arduino. The I2C module simplifies the wiring and reduces the number of pins required to connect the LCD to the Arduino. Use an I2C scanner of your choice to find the I2C address of the LCD module. I've included an Arduino sketch in the project to scan the I2C bus. The default address is `0x27`, but this can vary, so please check. The LCD I2C address can be configured in `src/constants.h`.

## LCD Character Display

The project utilizes a custom-defined character set to display information on a standard 4x20 character LCD. The custom character set, referred to as "bigNumbers," is designed to render large, clear, and easy-to-read characters on the LCD. It's based on the work by Adrian Jones, as described in the article: http://woodsgood.ca/projects/2015/02/27/4-line-lcd-big-numbers/

Characters are defined using an 8x5 dot matrix pattern for each segment, allowing for versatile and clear character representations. The character set is defined as follows:

![segment patterns](./images/segmentPatterns.png)

```c
// Definitions of custom LCD character segments
const uint8_t segmentPatterns[][8] PROGMEM = {
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F}, // char 1: top left triangle
    // ... (additional characters)
    {0x1F, 0x1F, 0x1E, 0x1C, 0x18, 0x18, 0x10, 0x10}  // char 7: middle right triangle
};
```
- 0xFE is used to denote a segment that is off.
- 0xFF represents a segment that is fully lit.

### Example - Rendering the Digit "0":
The digit "0" is constructed using twelve segments arranged in a 3x4 grid. Here's how the segments from segmentPatterns are utilized to form the digit "0" on the LCD:

|  LCD Line  |   segmentPatterns   |    bigNumbers     |
| ------ | ---------------------- | ---------------- |
| Line 1 | char 1, char 2, char 3 | 0x01, 0x02, 0x03 |
| Line 2 | ON, OFF, ON            | 0xFF, 0xFE, 0xFF |
| Line 3 | ON, OFF, ON            | 0xFF, 0xFE, 0xFF |
| Line 4 | char 4, char 6, char 5 | 0x04, 0x06, 0x05 |

LCD output for the digit "0":

```
..... ..... .....
..... ..... .....
..... ..... .....
..... ..... .....
....# ##### #....
..### ##### ###..
.#### ##### ####.
##### ##### #####

##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####

##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####
##### ..... #####

##### ##### #####
.#### ##### ####.
..### ##### ###..
....# ##### #....
..... ..... .....
..... ..... .....
..... ..... .....
..... ..... .....
```

## Getting Started

To get started with the Darkroom Timer:

1.  Install the Arduino IDE from the [official website](https://www.arduino.cc/en/software).  Ensure you have a version that supports the required libraries.
2.  Clone this repository to your local machine.
3.  Open `darkroom_timer.ino` with the Arduino IDE.
4.  **Install Required Libraries:** Use the Arduino IDE's Library Manager to install the following libraries:
    *   `MD_REncoder` (Rotary Encoder Library by MajicDesigns)
    *   `New-LiquidCrystal` (Liquid Crystal Library for Arduino and Chipkit by fmalpartida)
5.  Connect your Arduino to your computer via USB.
6.  Select the correct board and port in the Arduino IDE (Tools > Board and Tools > Port).
7.  Upload the sketch to your Arduino board.

## Configuration

The following settings can be configured in `src/constants.h`:

*   **Timer Settings:**
    *   `TimerConfig::MAX_DELAY`: The maximum timer delay in milliseconds.
    *   `TimerConfig::INCREMENT`: The timer increment in milliseconds.
    *   `TimerConfig::TURN_ENLARGER_LAMP_ON_DELAY`: The delay for the long-press functionality (manual lamp control).
    *   `TimerConfig::EEPROM_WRITE_DELAY`: The minimum delay between EEPROM writes (milliseconds).
*   **LCD Settings:**
    *   `LCDLayout4x20::LCD_ROWS`: Number of rows on the LCD.
    *   `LCDLayout4x20::LCD_COLS`: Number of columns on the LCD.
    *   `I2C_ADDRESS`: The I2C address of your LCD module.
*   **EEPROM Settings:**
    *   `EEPROM_START_ADDRESS`: Starting address for EEPROM wear leveling.
    *   `EEPROM_END_ADDRESS`: Ending address for EEPROM wear leveling.
    *    `EEPROM_MAGIC`: The magic number that indicates if the EEPROM is already formatted or not.
    *    `EEPROM_INIT_VALUE`: The value written as the default.
    *   `ADDRESS_TRACKER_ADDRESS`: Location in EEPROM to store the address pointer for wear leveling.

The following settings can be configured in `src/LampControl.h`, `src/encoderHandler.h` and `src/ButtonHandler.h`:

*   **Pin Assignments:**
    *  `RELAY_PIN`: The pin where the relay is connected.
    *   `MANUAL_LIGHT_PIN`: The pin where the manual light indicator (LED) is connected.
    *   `ROTARY_ENCODER_PIN_A`, `ROTARY_ENCODER_PIN_B`: The rotary encoder pins.
    *   `TIMER_BUTTON_PIN`: The timer start button pin.
    *   `ROTARY_ENCODER_BUTTON_PIN`: The rotary encoder's push button (resets timer to 0).

## Usage

-   Use the rotary encoder to set the desired time for development.  The time will be displayed on the LCD in seconds, with one decimal place (e.g., "12.5 SEC").
-   Start the timer by pressing the exposure button. LCD will turn off during the exposure to prevent light leaks.
-   The LCD will turn on when the development time is completed, and the relay will de-energize to turn off the enlarger lamp.
-   If you need to manually control the enlarger lamp, press and hold the exposure button for at least 2 seconds to turn the lamp on. Press the button again to turn it off. The manual light indicator will illuminate when the lamp is in manual mode.
-   Press the rotary encoder's push button to reset the timer to zero.

## Troubleshooting

*   **LCD Not Displaying:**
    *   Check the I2C address in `src/constants.h`.  Use an I2C scanner sketch to verify the address.
    *   Verify the wiring between the Arduino and the LCD.
    *   Ensure the `New-LiquidCrystal` library is correctly installed.
*   **Timer Not Working:**
    *   Verify the wiring of the rotary encoder and the push button.
    *   Check the pin assignments in `src/encoderHandler.h` and  `src/ButtonHandler.h`.
    *   Make sure the `MD_REncoder` library is correctly installed.
*   **EEPROM Issues:**
    *   If the LCD displays "EEPROM Failure! Replace ASAP!", the EEPROM has encountered too many bad blocks. The timer may still function, but data persistence is not guaranteed.
    *   Ensure the `EEPROM_START_ADDRESS` and `EEPROM_END_ADDRESS` are valid for your Arduino board.
*   **Enlarger Lamp Not Working:**
    *   Check the relay wiring.
    *   Verify the `RELAY_PIN` is correctly defined in `src/LampControl.h`.
    *   Test the relay separately to ensure it's functioning correctly.

## Contributing

Contributions to the Darkroom Timer project are welcome. To contribute:

1.  Fork the repository.
2.  Create a feature branch: `git checkout -b new-feature`
3.  Commit your changes: `git commit -am 'Add some feature'`
4.  Push to the branch: `git push origin new-feature`
5.  Submit a pull request.

## License

This project is open-source and available under the [MIT License](LICENSE.md).

## Credits

*   Based on the BIG FONT (4-line) LCD CHARACTERS project by Adrian Jones (http://woodsgood.ca/projects/2015/02/27/4-line-lcd-big-numbers/).
*   I2C scanner code custom-written by the project author.