#ifndef STATE_JSON_CODEC_H
#define STATE_JSON_CODEC_H

#include "state_service.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <optional>

// LED patches
struct LedModePatch {
    LedMode mode;
};

struct LedSolidColorPatch {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct LedBrightnessPatch {
    uint8_t brightness;
};

struct LedSpeedPatch {
    uint16_t speed;
};

struct SettingsPatch {
    uint16_t ledCount;
};

// WiFi patches
struct WiFiConnectPatch {
    String ssid;
    String password;
};

enum class WiFiScanStatus { Started, Running, Complete };

struct WiFiScanNetwork {
    String ssid;
    int32_t rssi = 0;
    int32_t channel = 0;
    String auth;
};

struct WiFiStaStatusResponse {
    String ssid;
    String ip;
    int32_t rssi;
};

struct WiFiApStatusResponse {
    String ssid;
    String ip;
};

struct WifiStatusResponse {
    std::optional<WiFiStaStatusResponse> sta;
    std::optional<WiFiApStatusResponse> ap;
};

static const uint8_t MAX_WIFI_SCAN_NETWORKS = 20;

struct WiFiScanResult {
    WiFiScanStatus status = WiFiScanStatus::Started;
    uint8_t count = 0;
    WiFiScanNetwork networks[MAX_WIFI_SCAN_NETWORKS];
};

void serializeLedState(JsonObject obj, const LedState &led);
void serializeWiFiState(JsonObject obj, const WiFiState &wifi);
void serializeSystemState(JsonObject obj, const SystemState &system);
void serializeWiFiStatus(JsonObject obj, const WifiStatusResponse &status);
String getWiFiStatusJson(const WifiStatusResponse &status);
// void serializeNetworkState(JsonObject obj, const NetworkState &network);
// void serializeDeviceState(JsonObject obj, const DeviceState &device);

// bool parseLedPatch(JsonObjectConst obj, LedPatch &out, String &error);
// bool parseWiFiPatch(JsonObjectConst obj, WiFiPatch &out, String &error);
// bool parseSystemPatch(JsonObjectConst obj, SystemPatch &out, String &error);
//
bool parseLedModePatch(const JsonObjectConst obj, LedModePatch &out,
                       String &error);
bool parseLedSolidColorPatch(const JsonObjectConst obj, LedSolidColorPatch &out,
                             String &error);
bool parseLedBrightnessPatch(const JsonObjectConst obj, LedBrightnessPatch &out,
                             String &error);
bool parseLedSpeedPatch(const JsonObjectConst obj, LedSpeedPatch &out,
                        String &error);
bool parseSettingsPatch(const JsonObjectConst obj, SettingsPatch &out,
                        String &error);

bool parseWiFiConnectPatch(const JsonObjectConst obj, WiFiConnectPatch &out,
                           String &error);

// String serializeWiFiScanResultJson(const WiFiScanResult &scanResult);

#endif // STATE_JSON_CODEC_H
