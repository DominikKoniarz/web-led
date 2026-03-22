#include "web_server.h"
#include "state_service.h"
#include "wifi_setup.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WiFi.h>

static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");
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

static String buildWsEnvelope(const char *type, const String *requestId,
                              const String *payloadJson,
                              const String *errorMessage) {
    JsonDocument doc;
    doc["type"] = type;

    if (requestId != nullptr && requestId->length() > 0) {
        doc["request_id"] = *requestId;
    }

    if (payloadJson != nullptr) {
        JsonDocument payloadDoc;
        DeserializationError payloadErr =
            deserializeJson(payloadDoc, *payloadJson);
        if (!payloadErr && payloadDoc.is<JsonObjectConst>()) {
            doc["payload"] = payloadDoc.as<JsonObjectConst>();
        }
    }

    if (errorMessage != nullptr) {
        doc["error"] = *errorMessage;
    }

    String out;
    serializeJson(doc, out);
    return out;
}

static void sendWsAck(AsyncWebSocketClient *client, const String *requestId,
                      const String *payloadJson = nullptr) {
    client->text(buildWsEnvelope("ack", requestId, payloadJson, nullptr));
}

static void sendWsError(AsyncWebSocketClient *client, const String *requestId,
                        const String &message) {
    client->text(buildWsEnvelope("error", requestId, nullptr, &message));
}

static String buildStateSyncEnvelope() {
    String statePayload = stateServiceFullJson();
    return buildWsEnvelope("state.sync", nullptr, &statePayload, nullptr);
}

static void broadcastStateSync() { notifyClients(stateServiceFullJson()); }

static bool parseRequestId(JsonObjectConst envelope, String &out) {
    if (!envelope["request_id"].is<const char *>()) {
        return false;
    }

    out = envelope["request_id"].as<String>();
    return true;
}

static bool extractPayloadObject(JsonObjectConst envelope,
                                 JsonVariantConst &outPayload, String &error) {
    JsonVariantConst payload = envelope["payload"];
    if (payload.isNull()) {
        error = "payload is required";
        return false;
    }

    if (!payload.is<JsonObjectConst>()) {
        error = "payload must be a JSON object";
        return false;
    }

    outPayload = payload;
    return true;
}

static void handleWsStateGet(AsyncWebSocketClient *client,
                             const String *requestId) {
    String payload = stateServiceFullJson();
    sendWsAck(client, requestId, &payload);
}

static void handleWsLedSet(AsyncWebSocketClient *client,
                           JsonObjectConst envelope, const String *requestId) {
    JsonVariantConst patch;
    String error;
    if (!extractPayloadObject(envelope, patch, error)) {
        sendWsError(client, requestId, error);
        return;
    }

    if (!stateServiceApplyLedPatch(patch, error)) {
        sendWsError(client, requestId, error);
        return;
    }

    String payload = stateServiceLedJson();
    sendWsAck(client, requestId, &payload);
    broadcastStateSync();
}

static void handleWsWiFiSet(AsyncWebSocketClient *client,
                            JsonObjectConst envelope, const String *requestId) {
    JsonVariantConst patch;
    String error;
    if (!extractPayloadObject(envelope, patch, error)) {
        sendWsError(client, requestId, error);
        return;
    }

    if (!stateServiceApplyWiFiPatch(patch, error)) {
        sendWsError(client, requestId, error);
        return;
    }

    JsonObjectConst obj = patch.as<JsonObjectConst>();
    if (obj["ssid"].is<const char *>()) {
        String ssid = obj["ssid"].as<String>();
        String password = obj["password"].is<const char *>()
                              ? obj["password"].as<String>()
                              : "";

        if (!wifiStartProvisioningConnect(ssid, password, error)) {
            sendWsError(client, requestId, error);
            return;
        }
    }

    String payload = stateServiceWiFiJson();
    sendWsAck(client, requestId, &payload);
    broadcastStateSync();
}

static void handleWsSystemSet(AsyncWebSocketClient *client,
                              JsonObjectConst envelope,
                              const String *requestId) {
    JsonVariantConst patch;
    String error;
    if (!extractPayloadObject(envelope, patch, error)) {
        sendWsError(client, requestId, error);
        return;
    }

    if (!stateServiceApplySystemPatch(patch, error)) {
        sendWsError(client, requestId, error);
        return;
    }

    String payload = stateServiceSystemJson();
    sendWsAck(client, requestId, &payload);
    broadcastStateSync();
}

static void handleWsMessage(AsyncWebSocketClient *client,
                            const String &message) {
    JsonDocument doc;
    DeserializationError deserialization = deserializeJson(doc, message);
    if (deserialization) {
        String error = String("Invalid JSON: ") + deserialization.c_str();
        sendWsError(client, nullptr, error);
        return;
    }

    if (!doc.is<JsonObjectConst>()) {
        sendWsError(client, nullptr, "Envelope must be a JSON object");
        return;
    }

    JsonObjectConst envelope = doc.as<JsonObjectConst>();
    if (!envelope["type"].is<const char *>()) {
        sendWsError(client, nullptr, "type is required");
        return;
    }

    String requestId;
    const String *requestIdPtr =
        parseRequestId(envelope, requestId) ? &requestId : nullptr;

    String type = envelope["type"].as<String>();
    if (type == "state.get") {
        handleWsStateGet(client, requestIdPtr);
        return;
    }

    if (type == "led.set") {
        handleWsLedSet(client, envelope, requestIdPtr);
        return;
    }

    if (type == "wifi.set") {
        handleWsWiFiSet(client, envelope, requestIdPtr);
        return;
    }

    if (type == "system.set") {
        handleWsSystemSet(client, envelope, requestIdPtr);
        return;
    }

    sendWsError(client, requestIdPtr, "Unknown message type");
}

static void onWebSocketEvent(AsyncWebSocket *server,
                             AsyncWebSocketClient *client, AwsEventType type,
                             void *arg, uint8_t *data, size_t len) {
    (void)server;

    if (type == WS_EVT_CONNECT) {
        Serial.printf("[ws] client connected id=%u total=%u\n", client->id(),
                      ws.count());
        client->text(buildStateSyncEnvelope());
        return;
    }

    if (type == WS_EVT_DISCONNECT) {
        Serial.printf("[ws] client disconnected id=%u total=%u\n", client->id(),
                      ws.count());
        return;
    }

    if (type != WS_EVT_DATA) {
        return;
    }

    AwsFrameInfo *info = reinterpret_cast<AwsFrameInfo *>(arg);
    if (info == nullptr || info->opcode != WS_TEXT) {
        return;
    }

    // Only process complete text frames to keep parser behavior predictable.
    if (!(info->final && info->index == 0 && info->len == len)) {
        sendWsError(client, nullptr, "Fragmented messages are not supported");
        return;
    }

    String message;
    message.reserve(len);
    for (size_t i = 0; i < len; i++) {
        message += static_cast<char>(data[i]);
    }

    handleWsMessage(client, message);
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
            broadcastStateSync();
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
            broadcastStateSync();
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
            if (!stateServiceApplyWiFiPatch(payload, error)) {
                sendJsonError(request, 400, error);
                return;
            }

            JsonObjectConst obj = payload.as<JsonObjectConst>();
            if (obj["ssid"].is<const char *>()) {
                String ssid = obj["ssid"].as<String>();
                String password = obj["password"].is<const char *>()
                                      ? obj["password"].as<String>()
                                      : "";
                if (!wifiStartProvisioningConnect(ssid, password, error)) {
                    sendJsonError(request, 400, error);
                    return;
                }
            }

            String response = stateServiceWiFiJson();
            broadcastStateSync();
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
            broadcastStateSync();
            request->send(200, "application/json", response);
        });

    ws.onEvent(onWebSocketEvent);
    server.addHandler(&ws);

    server.begin();

    lastBroadcastPayload = buildStateSyncEnvelope();
    Serial.println("HTTP/WebSocket server started on port 80 (async)");
}

void notifyClients(const String &payload) {
    lastBroadcastPayload =
        buildWsEnvelope("state.sync", nullptr, &payload, nullptr);
    ws.textAll(lastBroadcastPayload);
}

void cleanupClients() { ws.cleanupClients(); }

size_t getConnectedClients() { return ws.count(); }
