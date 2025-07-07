#include "Arduino.h"

static unsigned long mock_time = 0;

unsigned long millis() {
    return mock_time;
}

void set_mock_millis(unsigned long time) {
    mock_time = time;
}