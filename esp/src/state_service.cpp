#include "state_service.h"
#include "state_json_codec.h"
#include <Arduino.h>

static AppState gState;

static String serializeDoc(JsonDocument &doc) {
    String out;
    serializeJson(doc, out);
    return out;
}

void stateServiceInitDefaults() {
    gState.led.mode = LedMode::Solid;
    gState.led.brightnessPercent = 50;
    gState.led.speedPercent = 100;
    gState.led.solidColor = 0x00FFFFFF;

    gState.wifi.ssid = "";
    gState.wifi.ip = "0.0.0.0";
    gState.wifi.rssi = 0;

    gState.network.dhcpEnabled = true;
    gState.network.ip = "0.0.0.0";
    gState.network.subnet = "255.255.255.0";

    gState.system.ledCount = 30;
}

const AppState &stateServiceGet() { return gState; }

// LED
String getLedJson() {
    JsonDocument doc;
    JsonObject led = doc.to<JsonObject>();
    serializeLedState(led, gState.led);
    return serializeDoc(doc);
}

void updateLedMode(LedMode mode) { gState.led.mode = mode; }
void updateLedBrightness(uint8_t brightness) {
    gState.led.brightnessPercent = brightness;
}
void updateLedSpeed(uint16_t speed) { gState.led.speedPercent = speed; }
void updateLedSolidColor(uint32_t solidColor) {
    gState.led.solidColor = solidColor;
}

// WiFi
String getWiFiJson() {
    JsonDocument doc;
    JsonObject wifi = doc.to<JsonObject>();
    serializeWiFiState(wifi, gState.wifi);
    return serializeDoc(doc);
}

void updateWiFiState(const String &ssid, const String &ip, int32_t rssi) {
    gState.wifi.ssid = ssid;
    gState.wifi.ip = ip;
    gState.wifi.rssi = rssi;
}
