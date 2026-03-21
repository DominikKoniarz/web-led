#ifndef STATE_JSON_CODEC_H
#define STATE_JSON_CODEC_H

#include "state_service.h"
#include <Arduino.h>
#include <ArduinoJson.h>

struct LedPatch {
    bool hasEnabled = false;
    bool enabled = false;

    bool hasMode = false;
    LedMode mode = LedMode::Off;

    bool hasBrightness = false;
    uint8_t brightness = 0;

    bool hasSpeed = false;
    uint16_t speed = 0;

    bool hasLedCount = false;
    uint16_t ledCount = 0;

    bool hasSolidColor = false;
    uint32_t solidColor = 0;
};

struct WiFiPatch {
    bool hasCredentials = false;
    String ssid;

    bool hasDhcpEnabled = false;
    bool dhcpEnabled = true;

    bool hasIp = false;
    String ip;

    bool hasSubnet = false;
    String subnet;

    bool hasGateway = false;
    String gateway;

    bool hasDns1 = false;
    String dns1;

    bool hasDns2 = false;
    String dns2;
};

struct SystemPatch {
    bool hasDeviceName = false;
    String deviceName;
};

void serializeLedState(JsonObject obj, const LedState &led);
void serializeWiFiState(JsonObject obj, const WiFiState &wifi);
void serializeNetworkState(JsonObject obj, const NetworkState &network);
void serializeDeviceState(JsonObject obj, const DeviceState &device);

bool parseLedPatch(JsonObjectConst obj, LedPatch &out, String &error);
bool parseWiFiPatch(JsonObjectConst obj, WiFiPatch &out, String &error);
bool parseSystemPatch(JsonObjectConst obj, SystemPatch &out, String &error);

#endif // STATE_JSON_CODEC_H
