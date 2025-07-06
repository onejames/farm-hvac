#include "display_manager.h"
#include "config.h"
#include "version.h"
#include "logic/enum_converters.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <WiFi.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C

const unsigned long DISPLAY_UPDATE_INTERVAL_MS = 1000;

DisplayManager::DisplayManager()
    : _lastUpdateTime(0), _isSetup(false) {
    // The Adafruit_SSD1306 library uses the global Wire object, which is configured in setup().
    _display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
}

DisplayManager::~DisplayManager() {
    delete _display;
}

bool DisplayManager::setup() {
    // The I2C bus must be initialized before the display.
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

    if (!_display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        return false;
    }
    _isSetup = true;

    // Show splash screen
    _display->clearDisplay();
    _display->setTextSize(1);
    _display->setTextColor(SSD1306_WHITE);
    _display->setCursor(0, 0);
    _display->println(F("HVAC Monitor"));
    _display->println(F(FIRMWARE_VERSION));
    _display->display();
    delay(2000);
    return true;
}

void DisplayManager::update(const HVACData& data) {
    if (!_isSetup) return;

    unsigned long currentTime = millis();
    if (currentTime - _lastUpdateTime < DISPLAY_UPDATE_INTERVAL_MS) {
        return;
    }
    _lastUpdateTime = currentTime;

    drawStatusScreen(data);
}

void DisplayManager::drawStatusScreen(const HVACData& data) {
    _display->clearDisplay();
    _display->setTextSize(1);
    _display->setTextColor(SSD1306_WHITE);
    _display->setCursor(0, 0);

    _display->printf("R:%.1fC S:%.1fC dT:%.1fC\n", data.returnTempC, data.supplyTempC, data.deltaT);
    _display->printf("Fan: %s (%.1fA)\n", toString(data.fanStatus), data.fanAmps);
    _display->printf("Comp: %s (%.1fA)\n", toString(data.compressorStatus), data.compressorAmps);
    _display->printf("Pump: %s (%.1fA)\n", toString(data.geoPumpsStatus), data.geoPumpsAmps);
    
    _display->setCursor(0, 48);
    _display->printf("Alert: %s\n", toString(data.alertStatus));
    _display->printf("WiFi: %s", (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString().c_str() : "---");

    _display->display();
}