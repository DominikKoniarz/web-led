#include "wifi_setup.h"
#include "WiFi.h"
#include "secrets.h"
#include "state_service.h"
#include <Arduino.h>

static const String ssid = WIFI_SSID;
static const String password = WIFI_PASSWORD;
// static const String ssid = WIFI_SSID_2;
// static const String password = WIFI_PASSWORD_2;

static const String AP_SSID = "webled-setup";
static const String AP_PASSWORD = "webled-setup";

// static const unsigned short CONNECT_TIMEOUT_MS = 15000;

static const char *wifiModeToString(wifi_mode_t mode) {
    switch (mode) {
    case WIFI_MODE_NULL:
        return "null";
    case WIFI_MODE_STA:
        return "sta";
    case WIFI_MODE_AP:
        return "ap";
    case WIFI_MODE_APSTA:
        return "ap_sta";
    default:
        return "unknown";
    }
}

static void onWiFiEvent(WiFiEvent_t event) {
    switch (event) {
    // STA events
    case ARDUINO_EVENT_WIFI_READY:
        Serial.println("[WiFi] WiFi interface ready");
        break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:
        Serial.println("[WiFi] WiFi scan completed");
        break;
    case ARDUINO_EVENT_WIFI_STA_START:
        Serial.println("[WiFi] STA started");
        break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("[WiFi] STA connected to AP");
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.println("[WiFi] STA got IP: " + WiFi.localIP().toString());
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.println("[WiFi] STA disconnected (AutoReconnect will retry)");
        break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        Serial.println("[WiFi] STA lost IP");
        break;
    // AP events
    case ARDUINO_EVENT_WIFI_AP_START:
        Serial.println("[WiFi] AP started");
        break;
    case ARDUINO_EVENT_WIFI_AP_STOP:
        Serial.println("[WiFi] AP stopped");
        break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
        Serial.println("[WiFi] AP client connected");
        break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
        Serial.println("[WiFi] AP client disconnected");
        break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
        Serial.println("[WiFi] AP client got IP assigned");
        break;
    default:
        Serial.println("[WiFi] Unhandled event: " + String(event));
        break;
    }
}

void setupWiFi() {
    Serial.println("[WiFi] Initializing WiFi...");
    WiFi.onEvent(onWiFiEvent);

    WiFi.setAutoReconnect(true);

    const AppState &state = stateServiceGet();
    String staSsid = ssid;
    String staPassword = password;
    if (state.wifi.configured && state.wifi.ssid.length() > 0) {
        staSsid = state.wifi.ssid;
        staPassword = state.wifi.password;
        Serial.println("[WiFi] Using persisted STA credentials");
    } else {
        Serial.println("[WiFi] Using built-in (compiled) STA credentials");
    }

    WiFi.begin(staSsid, staPassword);
    Serial.println("[WiFi] Called WiFi.begin() for STA connection");

    // wait for connection, but don't block forever if credentials are wrong
    // unsigned long connectStartMs = millis();

    // while (WiFi.status() != WL_CONNECTED &&
    //        millis() - connectStartMs < CONNECT_TIMEOUT_MS) {
    //     delay(1000);
    //     Serial.print("[WiFi] Connecting to WiFi... " +
    //                  String((millis() - connectStartMs) / 1000) + "s\r");
    // }
    // Serial.println("[WiFi] STA Connection attempt finished");

    // TODO: work here and only enable it when needed

    if (WiFi.softAP(AP_SSID, AP_PASSWORD)) {
        Serial.println("[WiFi] AP started with SSID: " + AP_SSID);

        // set custom IP for AP
        IPAddress apIP(10, 0, 0, 1);
        IPAddress netMsk(255, 255, 255, 0);
        WiFi.softAPConfig(apIP, apIP, netMsk);
        Serial.println("[WiFi] AP Configured");
        Serial.println("[WiFi] AP IP address: " + WiFi.softAPIP().toString());
        Serial.println("[WiFi] AP Mask: " + WiFi.softAPSubnetMask().toString());

        if (WiFi.softAPbandwidth(WIFI_BW_HT20)) {
            Serial.println("[WiFi] AP bandwidth set to 20 MHz");
        } else {
            Serial.println("[WiFi] Failed to set AP bandwidth");
        }
    } else {
        Serial.println("[WiFi] Failed to start AP");
    }
}