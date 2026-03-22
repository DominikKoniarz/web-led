#include "LittleFS.h"
#include "state_service.h"
#include "web_server.h"
#include "wifi_setup.h"
#include <Arduino.h>

#include "WiFi.h"
#include "secrets.h"

#define LED_PIN 15

static const unsigned long CONNECT_TIMEOUT_MS = 20000;
static const unsigned long WIFI_SWITCH_INTERVAL_MS = 30000;

String ssid = WIFI_SSID;
String password = WIFI_PASSWORD;
unsigned long lastWiFiSwitchMs = 0;
bool usePrimaryNetwork = true;

static void onWiFiEvent(WiFiEvent_t event) {
    switch (event) {
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("[WiFi] STA connected to AP");
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.println("[WiFi] Got IP: " + WiFi.localIP().toString());
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.println("[WiFi] STA disconnected (AutoReconnect will retry)");
        break;
    default:
        break;
    }
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.begin(115200);
    delay(1000);

    if (!LittleFS.begin(true)) {
        Serial.println("LittleFS mount failed");
        return;
    }

    // stateServiceInitDefaults();

    // setupWiFi();

    // setupWebServer();

    // // 3 blinks to indicate successful setup
    // for (int i = 0; i < 3; i++) {
    //     digitalWrite(LED_PIN, HIGH);
    //     delay(100);
    //     digitalWrite(LED_PIN, LOW);
    //     delay(100);
    // }

    // TESTING

    // print WiFi status
    Serial.println("WiFi status: " + String(WiFi.status()));
    Serial.println("WiFi SSID: " + WiFi.SSID());
    Serial.println("WiFi IP: " + WiFi.localIP().toString());

    // connect to the WiFi network
    Serial.println("Connecting to WiFi...");
    WiFi.onEvent(onWiFiEvent);
    WiFi.mode(WIFI_AP_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(ssid.c_str(), password.c_str());

    // wait for connection, but don't block forever if credentials are wrong
    unsigned long connectStartMs = millis();
    while (WiFi.status() != WL_CONNECTED &&
           millis() - connectStartMs < CONNECT_TIMEOUT_MS) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi connected");
    } else {
        Serial.println("Initial WiFi connection timed out");
    }
    // print WiFi status
    Serial.println("WiFi status: " + String(WiFi.status()));
    Serial.println("WiFi SSID: " + WiFi.SSID());
    Serial.println("WiFi IP: " + WiFi.localIP().toString());

    // start AP (use AP+STA)
    WiFi.softAP("WebLED-AP", "password");
    // set custom IP for AP
    IPAddress apIP(10, 0, 0, 1);
    IPAddress netMsk(255, 255, 255, 0);
    WiFi.softAPConfig(apIP, apIP, netMsk);

    Serial.println("AP started with IP: " + WiFi.softAPIP().toString());
}

void loop() {
    delay(1000);

    unsigned long now = millis();
    if (now - lastWiFiSwitchMs >= WIFI_SWITCH_INTERVAL_MS) {
        lastWiFiSwitchMs = now;
        usePrimaryNetwork = !usePrimaryNetwork;

        ssid = usePrimaryNetwork ? WIFI_SSID : WIFI_SSID_2;
        password = usePrimaryNetwork ? WIFI_PASSWORD : WIFI_PASSWORD_2;

        Serial.println("Switching WiFi target to SSID: " + ssid);
        WiFi.disconnect(false, true);
        WiFi.begin(ssid.c_str(), password.c_str());
    }

    wl_status_t status = WiFi.status();
    Serial.println("Looping... WiFi status: " + String(status));

    // Keep loop non-invasive: WiFi library handles reconnect via AutoReconnect.
    if (status != WL_CONNECTED) {
        Serial.println("WiFi not connected; waiting for AutoReconnect");
    }

    // wifiSetupTick();

    // cleanupClients();

    // TESTING
}
