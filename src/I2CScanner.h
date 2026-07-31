#pragma once

#include <Arduino.h>

// Scans every valid I2C address and prints the results to the Serial Monitor.
// Returns true when expectedAddress responds.
bool ScanI2CBus(byte expectedAddress);
