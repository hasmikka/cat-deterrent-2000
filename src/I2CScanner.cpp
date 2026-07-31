#include "I2CScanner.h"

#include <Wire.h>

bool ScanI2CBus(byte expectedAddress) {
  byte deviceCount = 0;
  bool expectedDeviceFound = false;

  Serial.println(F("Scanning the I2C bus..."));

  // Valid 7-bit I2C device addresses normally run from 1 through 126.
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print(F("I2C device found at address 0x"));

      if (address < 0x10) {
        Serial.print(F("0"));
      }

      Serial.println(address, HEX);
      deviceCount++;

      if (address == expectedAddress) {
        expectedDeviceFound = true;
      }
    } else if (error == 4) {
      Serial.print(F("Unknown I2C error at address 0x"));

      if (address < 0x10) {
        Serial.print(F("0"));
      }

      Serial.println(address, HEX);
    }
  }

  if (deviceCount == 0) {
    Serial.println(F("No I2C devices found."));
  } else {
    Serial.print(F("Scan complete. Devices found: "));
    Serial.println(deviceCount);
  }

  return expectedDeviceFound;
}
