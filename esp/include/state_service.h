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
    uint16_t animationSpeed;
};

struct WiFiState {
    String ssid;
    String ip;
    int32_t rssi;
};

struct NetworkState {
    bool dhcpEnabled;
    String ip;
    String subnet;
};

struct SystemState {
    uint16_t ledCount;
};

struct AppState {
    LedState led;
    WiFiState wifi;
    NetworkState network;
    SystemState system;
};

void stateServiceInitDefaults();
const AppState &stateServiceGet();
bool stateServiceLoadFromFs();
bool stateServiceSaveToFs();
void stateServiceTick();
bool stateServiceIsDirty();
uint32_t stateServiceLastDirtyAtMs();
void stateServiceMarkDirty();

void updateLedMode(LedMode mode);
void updateLedBrightness(uint8_t brightness);
void updateLedSpeed(uint16_t speed);
void updateLedSolidColor(uint8_t red, uint8_t green, uint8_t blue);

void updateSystemLedCount(uint16_t ledCount);

#endif // STATE_SERVICE_H