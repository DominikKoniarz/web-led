#ifndef STATE_SERVICE_H
#define STATE_SERVICE_H

#include <Arduino.h>
#include <ArduinoJson.h>

enum class LedMode {
    Solid,
    Rainbow,
    Breathing,
    Chase,
    Sparkle,
    Fire,
    Wave,
    Off,
};

struct RgbColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct LedState {
    LedMode mode;
    uint8_t brightnessPercent;
    uint16_t speedPercent;
    RgbColor solidColor;
};

struct WiFiCredentials {
    bool configured;
    String ssid;
    String password;
};

struct SystemState {
    uint16_t ledCount;
};

struct AppState {
    LedState led;
    WiFiCredentials wifi;
    SystemState system;
};

void stateServiceInitDefaults();
const AppState &stateServiceGet();
bool stateServiceLoadFromFs();
bool stateServiceSaveToFs();
void stateServiceTick();
void stateServiceMarkDirty();

void updateLedMode(LedMode mode);
void updateLedBrightness(uint8_t brightness);
void updateLedSpeed(uint16_t speed);
void updateLedSolidColor(uint8_t red, uint8_t green, uint8_t blue);
void updateWiFiCredentials(const String &ssid, const String &password);

void updateSystemLedCount(uint16_t ledCount);

#endif // STATE_SERVICE_H