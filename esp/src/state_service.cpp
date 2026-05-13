#include "state_service.h"
#include "LittleFS.h"
#include "led_runtime.h"
#include <Arduino.h>
#include <ArduinoJson.h>

static AppState gState;
static bool gStateDirty = false;
static uint32_t gStateDirtyAtMs = 0;
static uint32_t gLastPersistAttemptAtMs = 0;

static constexpr const char *STATE_FILE_PATH = "/state.json";
static constexpr uint32_t STATE_FLUSH_INTERVAL_MS = 5000;

static uint16_t clampGlobalLedCount(uint16_t ledCount) {
    return ledCount > GLOBAL_LED_LIMIT ? GLOBAL_LED_LIMIT : ledCount;
}

static const char *toModeString(LedMode mode) {
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

static bool parseModeString(const String &value, LedMode &out) {
    if (value == "solid") {
        out = LedMode::Solid;
        return true;
    }
    if (value == "rainbow") {
        out = LedMode::Rainbow;
        return true;
    }
    if (value == "breathing") {
        out = LedMode::Breathing;
        return true;
    }
    if (value == "chase") {
        out = LedMode::Chase;
        return true;
    }
    if (value == "sparkle") {
        out = LedMode::Sparkle;
        return true;
    }
    if (value == "fire") {
        out = LedMode::Fire;
        return true;
    }
    if (value == "wave") {
        out = LedMode::Wave;
        return true;
    }
    if (value == "off") {
        out = LedMode::Off;
        return true;
    }

    return false;
}

static void serializeState(JsonObject root, const AppState &state) {
    JsonObject led = root["led"].to<JsonObject>();
    led["mode"] = toModeString(state.led.mode);
    led["brightnessPercent"] = state.led.brightnessPercent;
    led["speedPercent"] = state.led.speedPercent;
    led["red"] = state.led.solidColor.red;
    led["green"] = state.led.solidColor.green;
    led["blue"] = state.led.solidColor.blue;

    JsonObject wifi = root["wifi"].to<JsonObject>();
    wifi["configured"] = state.wifi.configured;
    if (state.wifi.configured) {
        wifi["ssid"] = state.wifi.ssid;
        wifi["password"] = state.wifi.password;
    }

    JsonObject system = root["system"].to<JsonObject>();
    system["ledCount"] = state.system.ledCount;
}

static void deserializeState(const JsonObjectConst root, AppState &state) {
    JsonObjectConst led = root["led"].as<JsonObjectConst>();
    if (!led.isNull()) {
        if (led["mode"].is<const char *>()) {
            LedMode parsed;
            if (parseModeString(led["mode"].as<String>(), parsed)) {
                state.led.mode = parsed;
            }
        }
        if (led["brightnessPercent"].is<uint8_t>()) {
            const uint8_t brightness = led["brightnessPercent"].as<uint8_t>();
            state.led.brightnessPercent = brightness > 100 ? 100 : brightness;
        }
        if (led["speedPercent"].is<uint16_t>()) {
            const uint16_t speed = led["speedPercent"].as<uint16_t>();
            state.led.speedPercent = speed > 100 ? 100 : speed;
        }
        if (led["red"].is<uint8_t>()) {
            state.led.solidColor.red = led["red"].as<uint8_t>();
        }
        if (led["green"].is<uint8_t>()) {
            state.led.solidColor.green = led["green"].as<uint8_t>();
        }
        if (led["blue"].is<uint8_t>()) {
            state.led.solidColor.blue = led["blue"].as<uint8_t>();
        }
    }

    JsonObjectConst wifi = root["wifi"].as<JsonObjectConst>();
    if (!wifi.isNull()) {
        bool configured = false;
        if (wifi["configured"].is<bool>()) {
            configured = wifi["configured"].as<bool>();
        } else if (wifi["ssid"].is<const char *>() ||
                   wifi["password"].is<const char *>()) {
            configured = true;
        }
        if (configured && wifi["ssid"].is<const char *>() &&
            wifi["password"].is<const char *>()) {
            state.wifi.configured = true;
            state.wifi.ssid = wifi["ssid"].as<String>();
            state.wifi.password = wifi["password"].as<String>();
        } else {
            state.wifi.configured = false;
            state.wifi.ssid = "";
            state.wifi.password = "";
        }
    }

    JsonObjectConst system = root["system"].as<JsonObjectConst>();
    if (!system.isNull() && system["ledCount"].is<uint16_t>()) {
        const uint16_t ledCount = system["ledCount"].as<uint16_t>();
        state.system.ledCount = clampGlobalLedCount(ledCount);
    }
}

void stateServiceInitDefaults() {
    gState.led.mode = LedMode::Rainbow;
    gState.led.brightnessPercent = 30;
    gState.led.speedPercent = 75;
    gState.led.solidColor = {.red = 15, .green = 128, .blue = 100};

    gState.wifi.configured = false;
    gState.wifi.ssid = "";
    gState.wifi.password = "";
    // gState.wifi.ssid = WIFI_SSID;
    // gState.wifi.password = WIFI_PASSWORD;

    gState.system.ledCount = 60;

    gStateDirty = false;
    gStateDirtyAtMs = 0;
    gLastPersistAttemptAtMs = 0;
}

const AppState &stateServiceGet() { return gState; }

bool stateServiceLoadFromFs() {
    if (!LittleFS.exists(STATE_FILE_PATH)) {
        return false;
    }

    File file = LittleFS.open(STATE_FILE_PATH, "r");
    if (!file) {
        return false;
    }

    JsonDocument doc;
    const DeserializationError err = deserializeJson(doc, file);
    file.close();
    if (err) {
        return false;
    }

    JsonObjectConst root = doc.as<JsonObjectConst>();
    if (root.isNull()) {
        return false;
    }

    deserializeState(root, gState);
    gStateDirty = false;
    gStateDirtyAtMs = 0;
    return true;
}

bool stateServiceSaveToFs() {
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    serializeState(root, gState);

    File file = LittleFS.open(STATE_FILE_PATH, "w");
    if (!file) {
        return false;
    }

    const size_t written = serializeJson(doc, file);
    file.close();
    if (written == 0) {
        return false;
    }

    Serial.printf("[state] Persisted to LittleFS: path=%s bytes=%u\n",
                  STATE_FILE_PATH, static_cast<unsigned>(written));

    gStateDirty = false;
    gStateDirtyAtMs = 0;
    return true;
}

void stateServiceTick() {
    if (!gStateDirty) {
        return;
    }

    const uint32_t nowMs = millis();
    if (nowMs - gStateDirtyAtMs < STATE_FLUSH_INTERVAL_MS) {
        return;
    }

    if (nowMs - gLastPersistAttemptAtMs < STATE_FLUSH_INTERVAL_MS) {
        return;
    }

    gLastPersistAttemptAtMs = nowMs;

    stateServiceSaveToFs();
}

void stateServiceMarkDirty() {
    gStateDirty = true;
    gStateDirtyAtMs = millis();
}

// LED
void updateLedMode(LedMode mode) {
    gState.led.mode = mode;

    stateServiceMarkDirty();
}
void updateLedBrightness(uint8_t brightness) {
    gState.led.brightnessPercent = brightness > 100 ? 100 : brightness;

    stateServiceMarkDirty();
}
void updateLedSpeed(uint16_t speed) {
    gState.led.speedPercent = speed > 100 ? 100 : speed;

    stateServiceMarkDirty();
}
void updateLedSolidColor(uint8_t red, uint8_t green, uint8_t blue) {
    gState.led.solidColor.red = red;
    gState.led.solidColor.green = green;
    gState.led.solidColor.blue = blue;

    stateServiceMarkDirty();
}
void updateWiFiCredentials(const String &ssid, const String &password) {
    gState.wifi.configured = true;
    gState.wifi.ssid = ssid;
    gState.wifi.password = password;

    stateServiceMarkDirty();
}
void updateSystemLedCount(uint16_t ledCount) {
    gState.system.ledCount = clampGlobalLedCount(ledCount);

    stateServiceMarkDirty();
}
