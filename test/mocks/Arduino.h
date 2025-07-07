#ifndef MOCK_ARDUINO_H
#define MOCK_ARDUINO_H

#include <cstdint>

// Mock implementation of Arduino's millis() function for native testing.
unsigned long millis();

// Test helper to control the mock time.
void set_mock_millis(unsigned long time);

#endif // MOCK_ARDUINO_H