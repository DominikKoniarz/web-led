#include "json_codec.h"
#include "led_runtime.h"
#include "state_service.h"

static const char *toString(LedMode mode) {
    switch (mode) {
    case LedMode::Solid:
        return "solid";
    case LedMode::Rainbow:
        return "rainbow";
    case LedMode::Breathing:
        return "breathing";
    case LedMode::Chase:
        return "chase";
    case LedMode::Sparkle:
        return "sparkle";
    case LedMode::Fire:
        return "fire";
    case LedMode::Wave:
        return "wave";
    case LedMode::Off:
        return "off";
    }

    return "off";
}

String serializeDoc(JsonDocument &doc) {
    String out;
    serializeJson(doc, out);
    return out;
}

void serializeLedState(JsonObject obj, const LedState &led) {
    obj["mode"] = toString(led.mode);
    obj["brightnessPercent"] = led.brightnessPercent;
    obj["speedPercent"] = led.speedPercent;
    obj["red"] = led.solidColor.red;
    obj["green"] = led.solidColor.green;
    obj["blue"] = led.solidColor.blue;
}

void serializeWiFiState(JsonObject obj, const WiFiState &wifi) {
    obj["ssid"] = wifi.ssid;
    obj["ip"] = wifi.ip;
    obj["rssi"] = wifi.rssi;
}

void serializeSystemState(JsonObject obj, const SystemState &system) {
    obj["ledCount"] = system.ledCount;
}

void serializeWiFiStatus(JsonObject obj, const WifiStatusResponse &status) {
    if (status.sta.has_value()) {
        JsonObject sta = obj["sta"].to<JsonObject>();
        sta["ssid"] = status.sta->ssid;
        sta["ip"] = status.sta->ip;
        sta["rssi"] = status.sta->rssi;
    } else {
        obj["sta"] = nullptr; // nullptr will serialize to null in JSON
    }

    if (status.ap.has_value()) {
        JsonObject ap = obj["ap"].to<JsonObject>();
        ap["ssid"] = status.ap->ssid;
        ap["ip"] = status.ap->ip;
    } else {
        obj["ap"] = nullptr; // nullptr will serialize to null in JSON
    }
}

String getLedJson() {
    const AppState &gState = stateServiceGet();

    JsonDocument doc;
    JsonObject led = doc.to<JsonObject>();
    serializeLedState(led, gState.led);
    return serializeDoc(doc);
}

String getSystemJson() {
    const AppState &gState = stateServiceGet();

    JsonDocument doc;
    JsonObject system = doc.to<JsonObject>();
    serializeSystemState(system, gState.system);
    return serializeDoc(doc);
}

String getWiFiStatusJson(const WifiStatusResponse &status) {
    JsonDocument doc;
    JsonObject obj = doc.to<JsonObject>();
    serializeWiFiStatus(obj, status);
    return serializeDoc(doc);
}

static bool parseLedMode(const String &mode, LedMode &out) {
    if (mode == "off") {
        out = LedMode::Off;
        return true;
    }
    if (mode == "solid") {
        out = LedMode::Solid;
        return true;
    }
    if (mode == "rainbow") {
        out = LedMode::Rainbow;
        return true;
    }
    if (mode == "breathing") {
        out = LedMode::Breathing;
        return true;
    }
    if (mode == "chase") {
        out = LedMode::Chase;
        return true;
    }
    if (mode == "sparkle") {
        out = LedMode::Sparkle;
        return true;
    }
    if (mode == "fire") {
        out = LedMode::Fire;
        return true;
    }
    if (mode == "wave") {
        out = LedMode::Wave;
        return true;
    }

    return false;
}

/*
    Example led mode patch JSON:
    {
        "mode": "rainbow"
    }

    modes: "off", "solid", "rainbow", "breathing", "chase", "sparkle", "fire",
   "wave"
*/
bool parseLedModePatch(const JsonObjectConst obj, LedModePatch &out,
                       String &error) {
    if (!obj["mode"].isNull()) {
        if (!obj["mode"].is<const char *>()) {
            error = "mode must be string";
            return false;
        }

        LedMode parsedMode;
        if (!parseLedMode(obj["mode"].as<String>(), parsedMode)) {
            error = "Unsupported mode. Supported modes are: off, solid, "
                    "rainbow, breathing, chase, sparkle, fire, wave";
            return false;
        }

        out.mode = parsedMode;
    } else {
        error = "mode is required";
        return false;
    }

    return true;
}

/*
    Example led patch JSON:
    {
        "red": 255,
        "green": 128,
        "blue": 64
    }
*/
bool parseLedSolidColorPatch(const JsonObjectConst obj, LedSolidColorPatch &out,
                             String &error) {
    if (obj["red"].isNull() || obj["green"].isNull() || obj["blue"].isNull()) {
        error = "red, green and blue are required";
        return false;
    }

    if (!obj["red"].is<uint8_t>() || !obj["green"].is<uint8_t>() ||
        !obj["blue"].is<uint8_t>()) {
        error = "red, green and blue must be integers in range 0..255";
        return false;
    }

    out.red = obj["red"].as<uint8_t>();
    out.green = obj["green"].as<uint8_t>();
    out.blue = obj["blue"].as<uint8_t>();

    return true;
}

/*
    Example led brightness patch JSON:
    {
        "brightnessPercent": 128
    }
    brightnessPercent must be 0..100
*/
bool parseLedBrightnessPatch(const JsonObjectConst obj, LedBrightnessPatch &out,
                             String &error) {
    if (!obj["brightnessPercent"].isNull()) {
        if (!obj["brightnessPercent"].is<uint8_t>()) {
            error = "brightnessPercent must be 0..100";
            return false;
        }
        out.brightness = obj["brightnessPercent"].as<uint8_t>();
    } else {
        error = "brightnessPercent is required";
        return false;
    }

    return true;
}

/*
    Example led speed patch JSON:
    {
        "speedPercent": 50
    }
    speedPercent must be 0..100
*/
bool parseLedSpeedPatch(const JsonObjectConst obj, LedSpeedPatch &out,
                        String &error) {
    if (!obj["speedPercent"].isNull()) {
        if (!obj["speedPercent"].is<uint16_t>()) {
            error = "speedPercent must be 0..100";
            return false;
        }
        out.speed = obj["speedPercent"].as<uint16_t>();
    } else {
        error = "speedPercent is required";
        return false;
    }

    return true;
}

/*
    Example WiFi connect patch JSON:
    {
        "ledCount": 30
    }
*/
bool parseSettingsPatch(const JsonObjectConst obj, SettingsPatch &out,
                        String &error) {
    if (!obj["ledCount"].isNull()) {
        if (!obj["ledCount"].is<uint16_t>()) {
            error = "ledCount must be a number";
            return false;
        }
        const uint16_t ledCount = obj["ledCount"].as<uint16_t>();
        if (ledCount > GLOBAL_LED_LIMIT) {
            error = String("ledCount must be 0..") + GLOBAL_LED_LIMIT;
            return false;
        }
        out.ledCount = ledCount;
    } else {
        error = "ledCount is required";
        return false;
    }

    return true;
}

/*
    Example WiFi connect patch JSON:
    {
        "ssid": "MyWiFi",
        "password": "MyPassword"
    }
*/
bool parseWiFiConnectPatch(const JsonObjectConst obj, WiFiConnectPatch &out,
                           String &error) {
    if (!obj["ssid"].isNull()) {
        if (!obj["ssid"].is<const char *>()) {
            error = "ssid must be string";
            return false;
        }
        out.ssid = obj["ssid"].as<String>();
    } else {
        error = "ssid is required";
        return false;
    }

    if (!obj["password"].isNull()) {
        if (!obj["password"].is<const char *>()) {
            error = "password must be string";
            return false;
        }
        out.password = obj["password"].as<String>();
    } else {
        error = "password is required";
        return false;
    }

    return true;
}
