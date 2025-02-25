/*
 * File: i2c_scanner.ino
 * Project: Darkroom Enlarger Timer
 * File Created: Monday, 17th February 2025 11:10:56 pm
 * Author: Andrei Grichine (andrei.grichine@gmail.com)
 * -----
 * Last Modified: Tuesday, 25th February 2025 9:04:29 am
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
 #include <Wire.h>

// Configuration options
#define SERIAL_BAUD 115200
#define SCAN_DELAY 5000
#define SHOW_ALL_ERRORS false  // Set to true to show all errors, false to hide common errors
#define USE_HASH_MAP true      // Use hash-based device lookup instead of linear search

// ESP32 specific configuration
#ifdef ESP32
  #define SDA_PIN_BUS1 21
  #define SCL_PIN_BUS1 22
  TwoWire Wire1 = TwoWire(1);
#endif

// Define known devices for identification based on their I2C address
struct DeviceInfo {
  byte address;
  const char* deviceName;
  byte registerToRead;  // Specific register to read for this device
  byte expectedBytes;   // How many bytes to request
};

// Using a more comprehensive device list with specific registers
const DeviceInfo knownDevices[] = {
  // PCF8574 GPIO Extenders (0x20-0x27)
  {0x20, "PCF8574 GPIO Extender", 0x00, 1},
  {0x21, "PCF8574 GPIO Extender", 0x00, 1},
  {0x22, "PCF8574 GPIO Extender", 0x00, 1},
  {0x23, "PCF8574 GPIO Extender", 0x00, 1},
  {0x24, "PCF8574 GPIO Extender", 0x00, 1},
  {0x25, "PCF8574 GPIO Extender", 0x00, 1},
  {0x26, "PCF8574 GPIO Extender", 0x00, 1},
  {0x27, "PCF8574 GPIO Extender", 0x00, 1},
  
  // Add more devices as needed
  {0x3C, "SSD1306 OLED Display", 0x00, 1},
  {0x3D, "SSD1306 OLED Display", 0x00, 1},
  {0x68, "DS3231/MPU6050", 0x75, 1},  // For MPU6050, register 0x75 returns the WHO_AM_I value
  {0x76, "BME280 Sensor", 0xD0, 1},   // Register 0xD0 contains the chip ID
  {0x77, "BMP180/BME280 Sensor", 0xD0, 1}
  // Add other devices as needed
};

// Global array to store if an address range has already been scanned
// This avoids repetitive error messages for empty addresses
bool addressScanned[128] = {false};

// Function to get device info from address - O(1) time if USE_HASH_MAP is true
const DeviceInfo* getDeviceInfo(byte address) {
  #if USE_HASH_MAP
    // Simple hash implementation for device lookup - much faster than linear search
    static const DeviceInfo* deviceMap[128] = {nullptr};
    static bool mapInitialized = false;
    
    // Initialize map once
    if (!mapInitialized) {
      for (unsigned int i = 0; i < sizeof(knownDevices) / sizeof(DeviceInfo); i++) {
        deviceMap[knownDevices[i].address] = &knownDevices[i];
      }
      mapInitialized = true;
    }
    
    return deviceMap[address];
  #else
    // Fall back to linear search - O(n) time
    for (unsigned int i = 0; i < sizeof(knownDevices) / sizeof(DeviceInfo); i++) {
      if (knownDevices[i].address == address) {
        return &knownDevices[i];
      }
    }
    return nullptr;
  #endif
}

// Function to print the name of the device based on the known device table
void printDeviceInfo(byte address, byte data = 0xFF) {
  const DeviceInfo* device = getDeviceInfo(address);
  
  Serial.print("Device found at 0x");
  Serial.print(address < 16 ? "0" : "");
  Serial.print(address, HEX);
  
  if (device) {
    Serial.print(": ");
    Serial.println(device->deviceName);
  } else {
    Serial.println(": Unknown device");
  }
  
  if (data != 0xFF) {
    Serial.print("  Device data: 0x");
    Serial.print(data < 16 ? "0" : "");
    Serial.println(data, HEX);
  }
}

// Function to scan the I2C bus and detect devices
void scanI2C(TwoWire &wire, const char* busName, bool resetScanHistory = false) {
  byte address;
  int nDevices = 0;
  unsigned long startTime = millis();
  
  // Reset scan history if requested
  if (resetScanHistory) {
    memset(addressScanned, 0, sizeof(addressScanned));
  }
  
  Serial.print("Scanning on ");
  Serial.print(busName);
  Serial.println("...");
  
  for (address = 1; address < 127; address++) {
    // Try to communicate with the device
    wire.beginTransmission(address);
    byte error = wire.endTransmission();
    
    if (error == 0) {
      // Device found!
      const DeviceInfo* device = getDeviceInfo(address);
      byte registerToRead = device ? device->registerToRead : 0x00;
      byte bytesToRequest = device ? device->expectedBytes : 1;
      
      // Try to read a register value from the device
      wire.beginTransmission(address);
      wire.write(registerToRead);
      wire.endTransmission();
      
      if (wire.requestFrom(address, bytesToRequest) > 0) {
        byte data = wire.read();
        printDeviceInfo(address, data);
      } else {
        printDeviceInfo(address);
      }
      
      nDevices++;
      addressScanned[address] = true;
    } 
    else if (SHOW_ALL_ERRORS || 
            (error != 2 && error != 5)) {  // Error 2 is "address not found" (common), 5 is "timeout"
      // Only display errors that aren't the common "device not present" errors
      if (!addressScanned[address]) {
        Serial.print("Error code ");
        Serial.print(error);
        Serial.print(" at address 0x");
        Serial.print(address < 16 ? "0" : "");
        Serial.println(address, HEX);
        addressScanned[address] = true;
      }
    }
  }
  
  if (nDevices == 0) {
    Serial.print("No I2C devices found on ");
    Serial.println(busName);
  } else {
    Serial.print("Found ");
    Serial.print(nDevices);
    Serial.print(" device(s) on ");
    Serial.println(busName);
  }
  
  unsigned long duration = millis() - startTime;
  Serial.print("Scan completed in ");
  Serial.print(duration);
  Serial.println(" ms");
  Serial.println();
}

void setup() {
  // Initialize serial and wait for monitor
  Serial.begin(SERIAL_BAUD);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  Serial.println("\nI2C Scanner - Enhanced Version");
  
  // Initialize default I2C bus
  Wire.begin();
  Wire.setClock(100000);  // Standard 100kHz I2C clock speed
  
  #ifdef ESP32
    // Initialize second I2C bus for ESP32
    Wire1.begin(SDA_PIN_BUS1, SCL_PIN_BUS1);
    Wire1.setClock(100000);
  #endif
  
  // Reset scan history on startup
  memset(addressScanned, 0, sizeof(addressScanned));
}

void loop() {
  // Scan the primary I2C bus
  scanI2C(Wire, "Bus 0");
  
  #ifdef ESP32
    // Scan the secondary I2C bus if on ESP32
    scanI2C(Wire1, "Bus 1");
  #endif
  
  // Reset scan history between full scans
  memset(addressScanned, 0, sizeof(addressScanned));
  
  Serial.println("----------------------");
  Serial.print("Waiting ");
  Serial.print(SCAN_DELAY / 1000);
  Serial.println(" seconds before next scan...");
  Serial.println("----------------------\n");
  
  delay(SCAN_DELAY);
}