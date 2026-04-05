#include "state_json_codec.h"
#include "led_runtime.h"
#include "state_service.h"

// static const char *toString(WiFiModeState mode) {
//     switch (mode) {
//     case WiFiModeState::Boot:
//         return "boot";
//     case WiFiModeState::Sta:
//         return "sta";
//     case WiFiModeState::Ap:
//         return "ap";
//     case WiFiModeState::ApSta:
//         return "ap_sta";
//     }

//     return "boot";
// }

// static const char *toString(WiFiLinkState link) {
//     switch (link) {
//     case WiFiLinkState::Idle:
//         return "idle";
//     case WiFiLinkState::Connecting:
//         return "connecting";
//     case WiFiLinkState::Connected:
//         return "connected";
//     case WiFiLinkState::Disconnected:
//         return "disconnected";
//     case WiFiLinkState::Failed:
//         return "failed";
//     }

//     return "idle";
// }

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

// static bool parseLedMode(const String &mode, LedMode &out) {
//     if (mode == "off") {
//         out = LedMode::Off;
//         return true;
//     }
//     if (mode == "solid") {
//         out = LedMode::Solid;
//         return true;
//     }
//     if (mode == "rainbow") {
//         out = LedMode::Rainbow;
//         return true;
//     }
//     if (mode == "breathing") {
//         out = LedMode::Breathing;
//         return true;
//     }

//     return false;
// }

// static const char *toString(WiFiScanStatus status) {
//     switch (status) {
//     case WiFiScanStatus::Started:
//         return "started";
//     case WiFiScanStatus::Running:
//         return "running";
//     case WiFiScanStatus::Complete:
//         return "complete";
//     }

//     return "started";
// }

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

// for now, maybe change later
void serializeWiFiStatus(JsonObject obj, const WifiStatusResponse &status) {
    if (status.sta.has_value()) {
        JsonObject sta = obj["sta"].to<JsonObject>();
        sta["ssid"] = status.sta->ssid;
        sta["ip"] = status.sta->ip;
        sta["rssi"] = status.sta->rssi;
    } else {
        obj["sta"] = nullptr;
    }

    if (status.ap.has_value()) {
        JsonObject ap = obj["ap"].to<JsonObject>();
        ap["ssid"] = status.ap->ssid;
        ap["ip"] = status.ap->ip;
    } else {
        obj["ap"] = nullptr;
    }
}

String getWiFiStatusJson(const WifiStatusResponse &status) {
    JsonDocument doc;
    JsonObject obj = doc.to<JsonObject>();
    serializeWiFiStatus(obj, status);
    return serializeDoc(doc);
}

// void serializeWiFiState(JsonObject obj, const WiFiState &wifi) {
//     obj["mode"] = toString(wifi.mode);
//     obj["link"] = toString(wifi.link);
//     obj["ssid"] = wifi.ssid;
//     obj["ip"] = wifi.ip;
//     obj["rssi"] = wifi.rssi;
//     obj["has_credentials"] = wifi.hasCredentials;
// }

// void serializeNetworkState(JsonObject obj, const NetworkState &network) {
//     obj["dhcp_enabled"] = network.dhcpEnabled;
//     obj["ip"] = network.ip;
//     obj["subnet"] = network.subnet;
//     obj["gateway"] = network.gateway;
//     obj["dns1"] = network.dns1;
//     obj["dns2"] = network.dns2;
// }

// void serializeDeviceState(JsonObject obj, const DeviceState &device) {
//     obj["name"] = device.name;
//     obj["firmware_version"] = device.firmwareVersion;
//     obj["hardware"] = device.hardware;
// }

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

// bool parseLedPatch(JsonObjectConst obj, LedPatch &out, String &error) {
//     if (!obj["enabled"].isNull()) {
//         if (!obj["enabled"].is<bool>()) {
//             error = "enabled must be boolean";
//             return false;
//         }
//         out.hasEnabled = true;
//         out.enabled = obj["enabled"].as<bool>();
//     }

//     if (!obj["mode"].isNull()) {
//         if (!obj["mode"].is<const char *>()) {
//             error = "mode must be string";
//             return false;
//         }

//         LedMode parsedMode;
//         if (!parseLedMode(obj["mode"].as<String>(), parsedMode)) {
//             error = "Unsupported mode";
//             return false;
//         }

//         out.hasMode = true;
//         out.mode = parsedMode;
//     }

//     if (!obj["brightness"].isNull()) {
//         if (!obj["brightness"].is<uint8_t>()) {
//             error = "brightness must be 0..255";
//             return false;
//         }
//         out.hasBrightness = true;
//         out.brightness = obj["brightness"].as<uint8_t>();
//     }

//     if (!obj["speed"].isNull()) {
//         if (!obj["speed"].is<uint16_t>()) {
//             error = "speed must be 0..65535";
//             return false;
//         }
//         out.hasSpeed = true;
//         out.speed = obj["speed"].as<uint16_t>();
//     }

//     if (!obj["led_count"].isNull()) {
//         if (!obj["led_count"].is<uint16_t>()) {
//             error = "led_count must be 0..65535";
//             return false;
//         }
//         out.hasLedCount = true;
//         out.ledCount = obj["led_count"].as<uint16_t>();
//     }

//     if (!obj["solid_color"].isNull()) {
//         if (!obj["solid_color"].is<uint32_t>()) {
//             error = "solid_color must be an integer";
//             return false;
//         }
//         out.hasSolidColor = true;
//         out.solidColor = obj["solid_color"].as<uint32_t>();
//     }

//     return true;
// }

// bool parseWiFiPatch(JsonObjectConst obj, WiFiPatch &out, String &error) {
//     if (!obj["ssid"].isNull() || !obj["password"].isNull()) {
//         if (!obj["ssid"].is<const char *>()) {
//             error = "ssid must be string";
//             return false;
//         }
//         out.hasCredentials = true;
//         out.ssid = obj["ssid"].as<String>();
//     }

//     if (!obj["dhcp_enabled"].isNull()) {
//         if (!obj["dhcp_enabled"].is<bool>()) {
//             error = "dhcp_enabled must be boolean";
//             return false;
//         }
//         out.hasDhcpEnabled = true;
//         out.dhcpEnabled = obj["dhcp_enabled"].as<bool>();
//     }

//     if (!obj["ip"].isNull()) {
//         if (!obj["ip"].is<const char *>()) {
//             error = "ip must be string";
//             return false;
//         }
//         out.hasIp = true;
//         out.ip = obj["ip"].as<String>();
//     }

//     if (!obj["subnet"].isNull()) {
//         if (!obj["subnet"].is<const char *>()) {
//             error = "subnet must be string";
//             return false;
//         }
//         out.hasSubnet = true;
//         out.subnet = obj["subnet"].as<String>();
//     }

//     if (!obj["gateway"].isNull()) {
//         if (!obj["gateway"].is<const char *>()) {
//             error = "gateway must be string";
//             return false;
//         }
//         out.hasGateway = true;
//         out.gateway = obj["gateway"].as<String>();
//     }

//     if (!obj["dns1"].isNull()) {
//         if (!obj["dns1"].is<const char *>()) {
//             error = "dns1 must be string";
//             return false;
//         }
//         out.hasDns1 = true;
//         out.dns1 = obj["dns1"].as<String>();
//     }

//     if (!obj["dns2"].isNull()) {
//         if (!obj["dns2"].is<const char *>()) {
//             error = "dns2 must be string";
//             return false;
//         }
//         out.hasDns2 = true;
//         out.dns2 = obj["dns2"].as<String>();
//     }

//     return true;
// }

// bool parseSystemPatch(JsonObjectConst obj, SystemPatch &out, String
// &error) {
//     if (!obj["device_name"].isNull()) {
//         if (!obj["device_name"].is<const char *>()) {
//             error = "device_name must be string";
//             return false;
//         }
//         String next = obj["device_name"].as<String>();
//         next.trim();
//         if (next.length() == 0) {
//             error = "device_name cannot be empty";
//             return false;
//         }
//         out.hasDeviceName = true;
//         out.deviceName = next;
//     }

//     return true;
// }

// String serializeWiFiScanResultJson(const WiFiScanResult &scanResult) {
//     JsonDocument doc;
//     doc["status"] = toString(scanResult.status);
//     doc["count"] = scanResult.count;

//     JsonArray networks = doc["networks"].to<JsonArray>();
//     if (scanResult.status == WiFiScanStatus::Complete) {
//         for (uint8_t i = 0; i < scanResult.count; i++) {
//             JsonObject network = networks.add<JsonObject>();
//             network["ssid"] = scanResult.networks[i].ssid;
//             network["rssi"] = scanResult.networks[i].rssi;
//             network["channel"] = scanResult.networks[i].channel;
//             network["auth"] = scanResult.networks[i].auth;
//         }
//     }

//     String payload;
//     serializeJson(doc, payload);
//     return payload;
// }
