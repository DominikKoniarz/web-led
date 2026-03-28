#include "wifi_setup.h"
#include "WiFi.h"
#include "secrets.h"
#include "state_json_codec.h"
#include <Arduino.h>

static const String ssid = WIFI_SSID;
static const String password = WIFI_PASSWORD;
// static const String ssid = WIFI_SSID_2;
// static const String password = WIFI_PASSWORD_2;

static const String AP_SSID = "webled-setup";
static const String AP_PASSWORD = "webled-setup";

static const unsigned short CONNECT_TIMEOUT_MS = 15000;

static void onWiFiEvent(WiFiEvent_t event) {
    switch (event) {
    // STA events
    case ARDUINO_EVENT_WIFI_STA_START:
        Serial.println("[WiFi] STA started");
        break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("[WiFi] STA connected to AP");
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.println("[WiFi] STA got IP: " + WiFi.localIP().toString());

        updateWiFiState(ssid, WiFi.localIP().toString(), WiFi.RSSI());
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.println("[WiFi] STA disconnected (AutoReconnect will retry)");
        break;
    // AP events
    case ARDUINO_EVENT_WIFI_AP_START:
        Serial.println("[WiFi] AP started");
        break;
    default:
        Serial.println("[WiFi] Unhandled event: " + String(event));
        break;
    }
}

void setupWiFi() {
    Serial.println("[WiFi] Initializing WiFi...");
    WiFi.onEvent(onWiFiEvent);
    WiFi.mode(WIFI_AP_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(ssid, password);

    // wait for connection, but don't block forever if credentials are wrong
    unsigned long connectStartMs = millis();

    while (WiFi.status() != WL_CONNECTED &&
           millis() - connectStartMs < CONNECT_TIMEOUT_MS) {
        delay(1000);
        Serial.print("[WiFi] Connecting to WiFi... " +
                     String((millis() - connectStartMs) / 1000) + "s\r");
    }
    Serial.println("[WiFi] STA Connection attempt finished");

    WiFi.softAP(AP_SSID, AP_PASSWORD);

    // set custom IP for AP
    IPAddress apIP(10, 0, 0, 1);
    IPAddress netMsk(255, 255, 255, 0);
    WiFi.softAPConfig(apIP, apIP, netMsk);
    Serial.println("[WiFi] AP Configured");
    Serial.println("[WiFi] AP IP address: " + WiFi.softAPIP().toString());
    Serial.println("[WiFi] AP Mask: " + WiFi.softAPSubnetMask().toString());
}