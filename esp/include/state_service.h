#ifndef STATE_SERVICE_H
#define STATE_SERVICE_H

#include <Arduino.h>
#include <ArduinoJson.h>

enum class WiFiModeState { Boot, Sta, Ap, ApSta };

enum class WiFiLinkState { Idle, Connecting, Connected, Disconnected, Failed };

enum class LedMode { Off, Solid, Rainbow, Breathing };

struct LedState {
    bool enabled;
    LedMode mode;
    uint8_t brightness;
    uint16_t speed;
    uint16_t ledCount;
    uint32_t solidColor;
};

struct WiFiState {
    WiFiModeState mode;
    WiFiLinkState link;
    String ssid;
    String ip;
    int32_t rssi;
    bool hasCredentials;
};

struct NetworkState {
    bool dhcpEnabled;
    String ip;
    String subnet;
    String gateway;
    String dns1;
    String dns2;
};

struct DeviceState {
    String name;
    String firmwareVersion;
    String hardware;
};

struct AppState {
    LedState led;
    WiFiState wifi;
    NetworkState network;
    DeviceState device;
};

void stateServiceInitDefaults();
const AppState &stateServiceGet();

void stateServiceUpdateWiFiStatus(WiFiModeState mode, WiFiLinkState link,
                                  const String &ssid, const String &ip,
                                  int32_t rssi);

void stateServiceSetWiFiCredentials(const String &ssid, bool hasCredentials);

bool stateServiceApplyLedPatch(JsonVariantConst patch, String &error);
bool stateServiceApplyWiFiPatch(JsonVariantConst patch, String &error);
bool stateServiceApplySystemPatch(JsonVariantConst patch, String &error);

String stateServiceLedJson();
String stateServiceWiFiJson();
String stateServiceSystemJson();
String stateServiceFullJson();

String stateServiceApiContractJson();

#endif // STATE_SERVICE_H