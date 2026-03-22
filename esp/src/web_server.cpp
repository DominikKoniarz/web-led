#include "web_server.h"
#include "state_json_codec.h"
#include "state_service.h"
#include "wifi_setup.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WiFi.h>

static AsyncWebServer server(80);
static String lastBroadcastPayload;

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

static void handleHealth(AsyncWebServerRequest *request) {
    request->send(200, "application/json", stateServiceSystemJson());
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

static void handleApiWifiGet(AsyncWebServerRequest *request) {
    request->send(200, "application/json", stateServiceWiFiJson());
}

static void handleApiWifiScanGet(AsyncWebServerRequest *request) {
    request->send(200, "application/json", wifiScanNetworksJson());
}

static void handleApiLedGet(AsyncWebServerRequest *request) {
    request->send(200, "application/json", stateServiceLedJson());
}

static void handleApiSystemGet(AsyncWebServerRequest *request) {
    request->send(200, "application/json", stateServiceSystemJson());
}

static void handleApiStateGet(AsyncWebServerRequest *request) {
    request->send(200, "application/json", stateServiceFullJson());
}

static void handleApiContractGet(AsyncWebServerRequest *request) {
    request->send(200, "application/json", stateServiceApiContractJson());
}

void setupWebServer() {
    DefaultHeaders::Instance().addHeader("Connection", "keep-alive");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    server.on("/", HTTP_GET, handleRoot);
    server.on("/health", HTTP_GET, handleHealth);
    server.on("/assets/*", HTTP_GET, handleAssets);

    server.on("/api/contracts", HTTP_GET, handleApiContractGet);
    server.on("/api/state", HTTP_GET, handleApiStateGet);
    server.on("/api/wifi/scan", HTTP_GET, handleApiWifiScanGet);
    server.on("/api/wifi", HTTP_GET, handleApiWifiGet);
    server.on("/api/led", HTTP_GET, handleApiLedGet);
    server.on("/api/system", HTTP_GET, handleApiSystemGet);

    server.on(
        "/api/led", HTTP_POST,
        [](AsyncWebServerRequest *request) { (void)request; }, nullptr,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
           size_t index, size_t total) {
            (void)index;
            (void)total;

            JsonDocument doc;
            String error;
            if (!parseJsonBody(data, len, doc, error)) {
                sendJsonError(request, 400, error);
                return;
            }

            if (!stateServiceApplyLedPatch(doc.as<JsonVariantConst>(), error)) {
                sendJsonError(request, 400, error);
                return;
            }

            String payload = stateServiceLedJson();
            notifyClients(payload);
            request->send(200, "application/json", payload);
        });

    server.on(
        "/api/wifi/connect", HTTP_POST,
        [](AsyncWebServerRequest *request) { (void)request; }, nullptr,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
           size_t index, size_t total) {
            (void)index;
            (void)total;

            JsonDocument doc;
            String error;
            if (!parseJsonBody(data, len, doc, error)) {
                sendJsonError(request, 400, error);
                return;
            }

            JsonObjectConst payload = doc.as<JsonObjectConst>();
            if (!payload["ssid"].is<const char *>()) {
                sendJsonError(request, 400, "ssid is required");
                return;
            }

            String ssid = payload["ssid"].as<String>();
            String password = payload["password"].is<const char *>()
                                  ? payload["password"].as<String>()
                                  : "";

            if (!wifiStartProvisioningConnect(ssid, password, error)) {
                sendJsonError(request, 400, error);
                return;
            }

            String response = stateServiceWiFiJson();
            notifyClients(stateServiceFullJson());
            request->send(200, "application/json", response);
        });

    server.on(
        "/api/wifi", HTTP_POST,
        [](AsyncWebServerRequest *request) { (void)request; }, nullptr,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
           size_t index, size_t total) {
            (void)index;
            (void)total;

            JsonDocument doc;
            String error;
            if (!parseJsonBody(data, len, doc, error)) {
                sendJsonError(request, 400, error);
                return;
            }

            JsonVariantConst payload = doc.as<JsonVariantConst>();
            if (!payload.is<JsonObjectConst>()) {
                sendJsonError(request, 400, "Payload must be a JSON object");
                return;
            }

            JsonObjectConst obj = payload.as<JsonObjectConst>();
            WiFiPatch wifiPatch;
            if (!parseWiFiPatch(obj, wifiPatch, error)) {
                sendJsonError(request, 400, error);
                return;
            }

            if (wifiPatch.hasCredentials) {
                String password = obj["password"].is<const char *>()
                                      ? obj["password"].as<String>()
                                      : "";
                if (!wifiStartProvisioningConnect(wifiPatch.ssid, password,
                                                  error)) {
                    sendJsonError(request, 400, error);
                    return;
                }
            }

            if (!stateServiceApplyWiFiPatch(payload, error)) {
                sendJsonError(request, 400, error);
                return;
            }

            String response = stateServiceWiFiJson();
            notifyClients(stateServiceFullJson());
            request->send(200, "application/json", response);
        });

    server.on(
        "/api/system", HTTP_POST,
        [](AsyncWebServerRequest *request) { (void)request; }, nullptr,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
           size_t index, size_t total) {
            (void)index;
            (void)total;

            JsonDocument doc;
            String error;
            if (!parseJsonBody(data, len, doc, error)) {
                sendJsonError(request, 400, error);
                return;
            }

            if (!stateServiceApplySystemPatch(doc.as<JsonVariantConst>(),
                                              error)) {
                sendJsonError(request, 400, error);
                return;
            }

            String response = stateServiceSystemJson();
            notifyClients(stateServiceFullJson());
            request->send(200, "application/json", response);
        });

    server.begin();

    Serial.println("HTTP server started on port 80 (async)");
}

void notifyClients(const String &payload) {
    // Phase 1 stores the latest payload while transport is added in Phase 3.
    lastBroadcastPayload = payload;
}

void cleanupClients() {
    // no-op for basic async server
}

size_t getConnectedClients() { return 0; }
