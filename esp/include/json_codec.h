#ifndef JSON_CODEC_H
#define JSON_CODEC_H

#include "state_service.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <optional>

static const uint8_t MAX_WIFI_SCAN_NETWORKS = 20;

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

struct SystemHealthResponse {
    float cpuTempC;
    uint32_t uptimeMs;
    uint32_t uptimeSec;
    uint32_t freeHeapBytes;
    uint32_t minFreeHeapBytes;
    uint16_t ledCount;
    String chipModel;
    uint32_t chipRevision;
};

String serializeDoc(JsonDocument &doc);

void serializeLedState(JsonObject obj, const LedState &led);
void serializeSystemState(JsonObject obj, const SystemState &system);
void serializeWiFiStatus(JsonObject obj, const WifiStatusResponse &status);
void serializeSystemHealth(JsonObject obj, const SystemHealthResponse &health);

String getLedJson();
String getSystemJson();
String getWiFiStatusJson(const WifiStatusResponse &status);
String getSystemHealthJson(const SystemHealthResponse &health);

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

#endif // JSON_CODEC_H
