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

struct LedState {
    LedMode mode;
    uint8_t brightnessPercent;
    uint16_t speedPercent;
    uint32_t solidColor;
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

// struct DeviceState {
//     String name;
//     String firmwareVersion;
//     String hardware;
// };

struct AppState {
    LedState led;
    WiFiState wifi;
    NetworkState network;
    SystemState system;
    // DeviceState device;
};

void stateServiceInitDefaults();
const AppState &stateServiceGet();

String getLedJson();
void updateLedMode(LedMode mode);
void updateLedBrightness(uint8_t brightness);
void updateLedSpeed(uint16_t speed);
void updateLedSolidColor(uint32_t solidColorHex);

String getWiFiJson();
void updateWiFiState(const String &ssid, const String &ip, int32_t rssi);

// void stateServiceUpdateWiFiStatus(const String &ssid, const String &ip,
//                                   int32_t rssi);

// void stateServiceSetWiFiCredentials(const String &ssid, bool hasCredentials);

// bool stateServiceApplyLedPatch(JsonVariantConst patch, String &error);
// bool stateServiceApplyWiFiPatch(JsonVariantConst patch, String &error);
// bool stateServiceApplySystemPatch(JsonVariantConst patch, String &error);

// String stateServiceLedJson();
// String stateServiceWiFiJson();
// String stateServiceSystemJson();
// String stateServiceFullJson();

// String stateServiceApiContractJson();

#endif // STATE_SERVICE_H