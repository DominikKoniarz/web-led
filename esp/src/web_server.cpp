#include "web_server.h"
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WiFi.h>

static AsyncWebServer server(80);

static void handleRoot(AsyncWebServerRequest *request) {
    File f = LittleFS.open("/index.html", "r");

    if (!f || f.isDirectory()) {
        if (f)
            f.close();
        request->send(500, "text/plain", "File not found");
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
    String json = "{";
    json += "\"uptime_s\":" + String(millis() / 1000);
    json += ",\"wifi_rssi\":" + String(WiFi.RSSI());
    json += ",\"wifi_ssid\":\"" + WiFi.SSID() + "\"";
    json += ",\"wifi_ip\":\"" + WiFi.localIP().toString() + "\"";
    json += ",\"free_heap\":" + String(ESP.getFreeHeap());
    json += "}";
    request->send(200, "application/json", json);
}

void setupWebServer() {
    DefaultHeaders::Instance().addHeader("Connection", "keep-alive");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    server.on("/", HTTP_GET, handleRoot);
    server.on("/health", HTTP_GET, handleHealth);
    server.on("/assets/*", HTTP_GET, handleAssets);
    server.begin();

    Serial.println("HTTP server started on port 80 (async)");
}

void notifyClients(const String &payload) {
    (void)payload;
    // TODO: Implement when adding WebSockets
}

void cleanupClients() {
    // no-op for basic async server
}

size_t getConnectedClients() { return 0; }
