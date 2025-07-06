#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "hvac_data.h"

// Forward declare the library class to avoid including it in the header,
// which keeps compile times faster and dependencies cleaner.
class Adafruit_SSD1306;

class DisplayManager {
public:
    DisplayManager();
    ~DisplayManager();

    bool setup();
    void update(const HVACData& data);

private:
    void drawStatusScreen(const HVACData& data);

    Adafruit_SSD1306* _display;
    unsigned long _lastUpdateTime;
    bool _isSetup;
};

#endif // DISPLAY_MANAGER_H