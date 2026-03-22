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
    gState.led.enabled = true;
    gState.led.mode = LedMode::Solid;
    gState.led.brightness = 128;
    gState.led.speed = 100;
    gState.led.ledCount = 30;
    gState.led.solidColor = 0x00FFFFFF;

    gState.wifi.mode = WiFiModeState::Boot;
    gState.wifi.link = WiFiLinkState::Idle;
    gState.wifi.ssid = "";
    gState.wifi.ip = "0.0.0.0";
    gState.wifi.rssi = 0;
    gState.wifi.hasCredentials = false;

    gState.network.dhcpEnabled = true;
    gState.network.ip = "0.0.0.0";
    gState.network.subnet = "255.255.255.0";
    gState.network.gateway = "0.0.0.0";
    gState.network.dns1 = "0.0.0.0";
    gState.network.dns2 = "0.0.0.0";

    gState.device.name = "webled";
    gState.device.firmwareVersion = "0.1.0";
    gState.device.hardware = "esp32-c6-devkitm-1";
}

const AppState &stateServiceGet() { return gState; }

void stateServiceUpdateWiFiStatus(WiFiModeState mode, WiFiLinkState link,
                                  const String &ssid, const String &ip,
                                  int32_t rssi) {
    gState.wifi.mode = mode;
    gState.wifi.link = link;
    gState.wifi.ssid = ssid;
    gState.wifi.ip = ip;
    gState.wifi.rssi = rssi;
}

void stateServiceSetWiFiCredentials(const String &ssid, bool hasCredentials) {
    gState.wifi.ssid = ssid;
    gState.wifi.hasCredentials = hasCredentials;
}

bool stateServiceApplyLedPatch(JsonVariantConst patch, String &error) {
    if (!patch.is<JsonObjectConst>()) {
        error = "Payload must be a JSON object";
        return false;
    }

    JsonObjectConst obj = patch.as<JsonObjectConst>();

    LedPatch ledPatch;
    if (!parseLedPatch(obj, ledPatch, error)) {
        return false;
    }

    if (ledPatch.hasEnabled)
        gState.led.enabled = ledPatch.enabled;
    if (ledPatch.hasMode)
        gState.led.mode = ledPatch.mode;
    if (ledPatch.hasBrightness)
        gState.led.brightness = ledPatch.brightness;
    if (ledPatch.hasSpeed)
        gState.led.speed = ledPatch.speed;
    if (ledPatch.hasLedCount)
        gState.led.ledCount = ledPatch.ledCount;
    if (ledPatch.hasSolidColor)
        gState.led.solidColor = ledPatch.solidColor;

    return true;
}

bool stateServiceApplyWiFiPatch(JsonVariantConst patch, String &error) {
    if (!patch.is<JsonObjectConst>()) {
        error = "Payload must be a JSON object";
        return false;
    }

    JsonObjectConst obj = patch.as<JsonObjectConst>();

    WiFiPatch wifiPatch;
    if (!parseWiFiPatch(obj, wifiPatch, error)) {
        return false;
    }

    if (wifiPatch.hasCredentials)
        stateServiceSetWiFiCredentials(wifiPatch.ssid,
                                       wifiPatch.ssid.length() > 0);
    if (wifiPatch.hasDhcpEnabled)
        gState.network.dhcpEnabled = wifiPatch.dhcpEnabled;
    if (wifiPatch.hasIp)
        gState.network.ip = wifiPatch.ip;
    if (wifiPatch.hasSubnet)
        gState.network.subnet = wifiPatch.subnet;
    if (wifiPatch.hasGateway)
        gState.network.gateway = wifiPatch.gateway;
    if (wifiPatch.hasDns1)
        gState.network.dns1 = wifiPatch.dns1;
    if (wifiPatch.hasDns2)
        gState.network.dns2 = wifiPatch.dns2;

    return true;
}

bool stateServiceApplySystemPatch(JsonVariantConst patch, String &error) {
    if (!patch.is<JsonObjectConst>()) {
        error = "Payload must be a JSON object";
        return false;
    }

    JsonObjectConst obj = patch.as<JsonObjectConst>();

    SystemPatch systemPatch;
    if (!parseSystemPatch(obj, systemPatch, error)) {
        return false;
    }

    if (systemPatch.hasDeviceName)
        gState.device.name = systemPatch.deviceName;

    return true;
}

String stateServiceLedJson() {
    JsonDocument doc;
    JsonObject led = doc.to<JsonObject>();
    serializeLedState(led, gState.led);
    return serializeDoc(doc);
}

String stateServiceWiFiJson() {
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    serializeWiFiState(root["wifi"].to<JsonObject>(), gState.wifi);
    serializeNetworkState(root["network"].to<JsonObject>(), gState.network);
    return serializeDoc(doc);
}

String stateServiceSystemJson() {
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    serializeDeviceState(root["device"].to<JsonObject>(), gState.device);
    root["uptime_s"] = millis() / 1000;
    root["free_heap"] = ESP.getFreeHeap();
    return serializeDoc(doc);
}

String stateServiceFullJson() {
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    serializeLedState(root["led"].to<JsonObject>(), gState.led);
    serializeWiFiState(root["wifi"].to<JsonObject>(), gState.wifi);
    serializeNetworkState(root["network"].to<JsonObject>(), gState.network);
    serializeDeviceState(root["device"].to<JsonObject>(), gState.device);
    root["uptime_s"] = millis() / 1000;
    return serializeDoc(doc);
}

String stateServiceApiContractJson() {
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    root["version"] = 1;

    JsonObject rest = root["rest"].to<JsonObject>();
    rest["wifi"] = "/api/wifi";
    rest["wifi_scan"] = "/api/wifi/scan";
    rest["wifi_connect"] = "/api/wifi/connect";
    rest["led"] = "/api/led";
    rest["system"] = "/api/system";

    JsonObject ws = root["websocket"].to<JsonObject>();
    ws["path"] = "/ws";

    JsonArray clientMessages = ws["client_messages"].to<JsonArray>();
    clientMessages.add("state.get");
    clientMessages.add("led.set");
    clientMessages.add("wifi.set");
    clientMessages.add("system.set");

    JsonArray serverMessages = ws["server_messages"].to<JsonArray>();
    serverMessages.add("state.sync");
    serverMessages.add("ack");
    serverMessages.add("error");

    JsonObject envelope = ws["envelope"].to<JsonObject>();
    envelope["required"] = "type";
    envelope["optional"] = "request_id,payload";

    return serializeDoc(doc);
}