#include "web_server.h"
#include "json_codec.h"
#include "state_service.h"
#include "wifi_setup.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <Esp.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <optional>

static AsyncWebServer server(80);
static unsigned long wifiScanCompletedAtMs = 0;

static const unsigned long WIFI_SCAN_RESULT_MAX_AGE_MS = 15000;

static void handleRoot(AsyncWebServerRequest *request) {
    File f = LittleFS.open("/index.html", "r");

    if (!f || f.isDirectory()) {
        if (f)
            f.close();
        request->send(404);
        return;
    }

    AsyncWebServerResponse *resp = request->beginChunkedResponse(
        "text/html",
        [f](uint8_t *buffer, size_t maxLen, size_t index) mutable -> size_t {
            int n = f.read(buffer, maxLen);
            if (n <= 0)
                f.close();
            return static_cast<size_t>(n > 0 ? n : 0);
        });

    request->send(resp);
}

static const char *contentTypeFromPath(const String &path) {
    if (path.endsWith(".js"))
        return "application/javascript";
    if (path.endsWith(".css"))
        return "text/css";
    if (path.endsWith(".woff2"))
        return "font/woff2";
    if (path.endsWith(".woff"))
        return "font/woff";
    if (path.endsWith(".ico"))
        return "image/x-icon";
    if (path.endsWith(".svg"))
        return "image/svg+xml";
    return "application/octet-stream";
}

static const char *wifiScanAuthToString(wifi_auth_mode_t authMode) {
    switch (authMode) {
    case WIFI_AUTH_OPEN:
        return "open";
    case WIFI_AUTH_WEP:
        return "wep";
    case WIFI_AUTH_WPA_PSK:
        return "wpa_psk";
    case WIFI_AUTH_WPA2_PSK:
        return "wpa2_psk";
    case WIFI_AUTH_WPA_WPA2_PSK:
        return "wpa_wpa2_psk";
    case WIFI_AUTH_WPA2_ENTERPRISE:
        return "wpa2_enterprise";
    case WIFI_AUTH_WPA3_PSK:
        return "wpa3_psk";
    case WIFI_AUTH_WPA2_WPA3_PSK:
        return "wpa2_wpa3_psk";
    case WIFI_AUTH_WAPI_PSK:
        return "wapi_psk";
    case WIFI_AUTH_OWE:
        return "owe";
    default:
        return "unknown";
    }
}

static bool startAsyncWifiScan(String &error) {
    wifiScanCompletedAtMs = 0;
    WiFi.scanDelete();

    int16_t started = WiFi.scanNetworks(true, true);
    if (started == WIFI_SCAN_RUNNING || started >= 0) {
        return true;
    }

    error = "Failed to start WiFi scan";
    return false;
}

static void handleAssets(AsyncWebServerRequest *request) {
    String path = request->url();
    int q = path.indexOf('?');

    if (q >= 0)
        path = path.substring(0, q);
    if (!path.startsWith("/assets/")) {
        request->send(404);
        return;
    }

    String pathGz = path + ".gz";

    File f = LittleFS.open(pathGz, "r");
    if (!f || f.isDirectory()) {
        if (f)
            f.close();
        request->send(404);
        return;
    }

    const char *contentType = contentTypeFromPath(path);

    AsyncWebServerResponse *resp = request->beginChunkedResponse(
        contentType,
        [f](uint8_t *buffer, size_t maxLen, size_t index) mutable -> size_t {
            int n = f.read(buffer, maxLen);
            if (n <= 0)
                f.close();
            return static_cast<size_t>(n > 0 ? n : 0);
        });

    resp->addHeader("Content-Encoding", "gzip");
    resp->addHeader("Cache-Control", "public, max-age=31536000, immutable");

    request->send(resp);
}

static void sendJsonError(AsyncWebServerRequest *request, int code,
                          const String &message) {
    JsonDocument doc;
    doc["error"] = message;
    String payload;
    serializeJson(doc, payload);
    request->send(code, "application/json", payload);
}

static bool parseJsonBody(uint8_t *data, size_t len, JsonDocument &doc,
                          String &error) {
    DeserializationError deserialization = deserializeJson(doc, data, len);
    if (deserialization) {
        error = String("Invalid JSON: ") + deserialization.c_str();
        return false;
    }
    return true;
}

static void handleLedsGet(AsyncWebServerRequest *request) {
    request->send(200, "application/json", getLedJson());
}

static void handleSettingsGet(AsyncWebServerRequest *request) {
    request->send(200, "application/json", getSystemJson());
}

static void handleLedsModePost(AsyncWebServerRequest *request, uint8_t *data,
                               size_t len, size_t index, size_t total) {
    (void)total;
    (void)index;

    JsonDocument doc;
    String error;

    if (!parseJsonBody(data, len, doc, error)) {
        sendJsonError(request, 400, error);
        return;
    }

    LedModePatch patch;
    if (!parseLedModePatch(doc.as<JsonObjectConst>(), patch, error)) {
        sendJsonError(request, 400, error);
        return;
    }

    updateLedMode(patch.mode);

    request->send(200, "application/json", getLedJson());
}

static void handleLedsSolidColorPost(AsyncWebServerRequest *request,
                                     uint8_t *data, size_t len, size_t index,
                                     size_t total) {
    (void)total;
    (void)index;

    JsonDocument doc;
    String error;

    if (!parseJsonBody(data, len, doc, error)) {
        sendJsonError(request, 400, error);
        return;
    }

    LedSolidColorPatch patch;
    if (!parseLedSolidColorPatch(doc.as<JsonObjectConst>(), patch, error)) {
        sendJsonError(request, 400, error);
        return;
    }

    updateLedMode(LedMode::Solid);
    updateLedSolidColor(patch.red, patch.green, patch.blue);

    request->send(200, "application/json", getLedJson());
}

static void handleLedsBrightnessPost(AsyncWebServerRequest *request,
                                     uint8_t *data, size_t len, size_t index,
                                     size_t total) {
    (void)total;
    (void)index;

    JsonDocument doc;
    String error;

    if (!parseJsonBody(data, len, doc, error)) {
        sendJsonError(request, 400, error);
        return;
    }

    LedBrightnessPatch patch;
    if (!parseLedBrightnessPatch(doc.as<JsonObjectConst>(), patch, error)) {
        sendJsonError(request, 400, error);
        return;
    }

    updateLedBrightness(patch.brightness);

    request->send(200, "application/json", getLedJson());
}

static void handleLedsSpeedPost(AsyncWebServerRequest *request, uint8_t *data,
                                size_t len, size_t index, size_t total) {
    (void)total;
    (void)index;

    JsonDocument doc;
    String error;

    if (!parseJsonBody(data, len, doc, error)) {
        sendJsonError(request, 400, error);
        return;
    }

    LedSpeedPatch patch;
    if (!parseLedSpeedPatch(doc.as<JsonObjectConst>(), patch, error)) {
        sendJsonError(request, 400, error);
        return;
    }

    updateLedSpeed(patch.speed);

    request->send(200, "application/json", getLedJson());
}

static void handleWifiConnectPost(AsyncWebServerRequest *request, uint8_t *data,
                                  size_t len, size_t index, size_t total) {
    (void)total;
    (void)index;

    JsonDocument doc;
    String error;

    if (!parseJsonBody(data, len, doc, error)) {
        sendJsonError(request, 400, error);
        return;
    }

    WiFiConnectPatch patch;
    if (!parseWiFiConnectPatch(doc.as<JsonObjectConst>(), patch, error)) {
        sendJsonError(request, 400, error);
        return;
    }

    request->send(204, "application/json");

    // brief delay to ensure response is sent before WiFi disconnects
    delay(1000);

    // TODO: dump data to the persistent storage
    WiFi.begin(patch.ssid.c_str(), patch.password.c_str());
}

static void handleSettingsPost(AsyncWebServerRequest *request, uint8_t *data,
                               size_t len, size_t index, size_t total) {
    (void)total;
    (void)index;

    JsonDocument doc;
    String error;

    if (!parseJsonBody(data, len, doc, error)) {
        sendJsonError(request, 400, error);
        return;
    }

    SettingsPatch patch;
    if (!parseSettingsPatch(doc.as<JsonObjectConst>(), patch, error)) {
        sendJsonError(request, 400, error);
        return;
    }

    updateSystemLedCount(patch.ledCount);

    request->send(200, "application/json", getSystemJson());
}

static void handleWifiStatusGet(AsyncWebServerRequest *request) {
    std::optional<WiFiStaStatusResponse> staResponse;
    std::optional<WiFiApStatusResponse> apResponse;

    if (WiFi.getMode() == WIFI_MODE_STA || WiFi.getMode() == WIFI_MODE_APSTA) {
        if (WiFi.status() == WL_CONNECTED) {
            WiFiStaStatusResponse sta;
            sta.ssid = WiFi.SSID();
            sta.ip = WiFi.localIP().toString();
            sta.rssi = WiFi.RSSI();
            staResponse = sta;
        }
    }

    if (WiFi.getMode() == WIFI_MODE_AP || WiFi.getMode() == WIFI_MODE_APSTA) {
        WiFiApStatusResponse ap;
        ap.ssid = WiFi.softAPSSID();
        ap.ip = WiFi.softAPIP().toString();
        apResponse = ap;
    }

    WifiStatusResponse response;

    response.sta = staResponse;
    response.ap = apResponse;

    request->send(200, "application/json", getWiFiStatusJson(response));
}

static void handleWifiScanGet(AsyncWebServerRequest *request) {
    auto scanState = WiFi.scanComplete();

    if (scanState == WIFI_SCAN_RUNNING) {
        JsonDocument doc;
        doc["status"] = "running";

        String payload;
        serializeJson(doc, payload);

        request->send(200, "application/json", payload);
        return;
    }

    if (scanState >= 0) {
        if (wifiScanCompletedAtMs == 0) {
            wifiScanCompletedAtMs = millis();
        }

        if ((millis() - wifiScanCompletedAtMs) > WIFI_SCAN_RESULT_MAX_AGE_MS) {
            String error;
            if (!startAsyncWifiScan(error)) {
                sendJsonError(request, 500, error);
                return;
            }

            JsonDocument doc;
            doc["status"] = "started";

            String payload;
            serializeJson(doc, payload);

            request->send(200, "application/json", payload);
            return;
        }

        JsonDocument doc;
        doc["status"] = "complete";
        doc["count"] = scanState;

        JsonArray networks = doc["networks"].to<JsonArray>();
        for (int16_t i = 0; i < scanState; i++) {
            JsonObject network = networks.add<JsonObject>();

            network["ssid"] =
                WiFi.SSID(i).length() > 0 ? WiFi.SSID(i) : "<hidden>";
            network["rssi"] = WiFi.RSSI(i);
            network["channel"] = WiFi.channel(i);
            network["auth"] = wifiScanAuthToString(WiFi.encryptionType(i));
        }

        String payload;
        serializeJson(doc, payload);
        request->send(200, "application/json", payload);

        // remove scan results to free memory
        WiFi.scanDelete();
        return;
    }

    String error;
    if (!startAsyncWifiScan(error)) {
        sendJsonError(request, 500, error);
        return;
    }

    JsonDocument doc;
    doc["status"] = "started";

    String payload;
    serializeJson(doc, payload);

    request->send(200, "application/json", payload);
}

static void handleSystemHealthGet(AsyncWebServerRequest *request) {
    const AppState &state = stateServiceGet();

    const uint32_t uptimeMs = static_cast<uint32_t>(millis());

    SystemHealthResponse health;
    health.cpuTempC = temperatureRead();
    health.uptimeMs = uptimeMs;
    health.uptimeSec = uptimeMs / 1000;
    health.freeHeapBytes = ESP.getFreeHeap();
    health.minFreeHeapBytes = ESP.getMinFreeHeap();
    health.ledCount = state.system.ledCount;
    health.chipModel = ESP.getChipModel();
    health.chipRevision = ESP.getChipRevision();

    request->send(200, "application/json", getSystemHealthJson(health));
}

void setupWebServer() {
    DefaultHeaders::Instance().addHeader("Connection", "keep-alive");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    server.on("/", HTTP_GET, handleRoot);
    // TODO: handle webmanifest (and favicon?)
    server.on("/assets/*", HTTP_GET, handleAssets);

    // leds
    server.on("/api/leds", HTTP_GET, handleLedsGet);
    server.on(
        "/api/leds/mode", HTTP_POST,
        [](AsyncWebServerRequest *request) { (void)request; }, nullptr,
        handleLedsModePost);
    server.on(
        "/api/leds/color", HTTP_POST,
        [](AsyncWebServerRequest *request) { (void)request; }, nullptr,
        handleLedsSolidColorPost);
    server.on(
        "/api/leds/brightness", HTTP_POST,
        [](AsyncWebServerRequest *request) { (void)request; }, nullptr,
        handleLedsBrightnessPost);
    server.on(
        "/api/leds/speed", HTTP_POST,
        [](AsyncWebServerRequest *request) { (void)request; }, nullptr,
        handleLedsSpeedPost);

    // wifi
    server.on(
        "/api/wifi/connect", HTTP_POST,
        [](AsyncWebServerRequest *request) { (void)request; }, nullptr,
        handleWifiConnectPost);
    server.on("/api/wifi/status", HTTP_GET, handleWifiStatusGet);
    server.on("/api/wifi/scan", HTTP_GET, handleWifiScanGet);

    // settings
    server.on("/api/settings", HTTP_GET, handleSettingsGet);
    server.on(
        "/api/settings", HTTP_POST,
        [](AsyncWebServerRequest *request) { (void)request; }, nullptr,
        handleSettingsPost);

    server.on("/api/system/health", HTTP_GET, handleSystemHealthGet);

    server.onNotFound(handleRoot);

    server.begin();

    Serial.println("[HTTP] Server started on port 80 (async)");
}
