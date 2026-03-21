#include "wifi_setup.h"
#include "secrets.h"
#include "state_service.h"
#include <WiFi.h>

static String gConfiguredSsid = WIFI_SSID;
static String gConfiguredPassword = WIFI_PASSWORD;

void setWiFiCredentials(const String &ssid, const String &password) {
    gConfiguredSsid = ssid;
    gConfiguredPassword = password;
    stateServiceSetWiFiCredentials(gConfiguredSsid,
                                   gConfiguredSsid.length() > 0);
}

void setupWiFi() {
    stateServiceSetWiFiCredentials(gConfiguredSsid,
                                   gConfiguredSsid.length() > 0);
    stateServiceUpdateWiFiStatus(WiFiModeState::Sta, WiFiLinkState::Connecting,
                                 gConfiguredSsid, "0.0.0.0", 0);

    WiFi.mode(WIFI_STA);
    // For env variables usage check SECRETS.md
    WiFi.begin(gConfiguredSsid.c_str(), gConfiguredPassword.c_str());

    Serial.println("WIFI_SSID: " + gConfiguredSsid);
    Serial.println("WIFI_PASSWORD: " + gConfiguredPassword);

    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());

    stateServiceUpdateWiFiStatus(WiFiModeState::Sta, WiFiLinkState::Connected,
                                 WiFi.SSID(), WiFi.localIP().toString(),
                                 WiFi.RSSI());
}

void reconnectWiFi() {
    if (WiFi.status() == WL_CONNECTED) {
        stateServiceUpdateWiFiStatus(WiFiModeState::Sta,
                                     WiFiLinkState::Connected, WiFi.SSID(),
                                     WiFi.localIP().toString(), WiFi.RSSI());
        return;
    }

    stateServiceUpdateWiFiStatus(WiFiModeState::Sta,
                                 WiFiLinkState::Disconnected, gConfiguredSsid,
                                 "0.0.0.0", 0);

    Serial.println("WiFi lost. Attempting reconnect...");
    WiFi.disconnect();
    WiFi.begin(gConfiguredSsid.c_str(), gConfiguredPassword.c_str());

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.print("Reconnected! IP: ");
        Serial.println(WiFi.localIP());
        stateServiceUpdateWiFiStatus(WiFiModeState::Sta,
                                     WiFiLinkState::Connected, WiFi.SSID(),
                                     WiFi.localIP().toString(), WiFi.RSSI());
    } else {
        Serial.println();
        Serial.println("Reconnect failed. Will retry next loop.");
        stateServiceUpdateWiFiStatus(WiFiModeState::Sta, WiFiLinkState::Failed,
                                     gConfiguredSsid, "0.0.0.0", 0);
    }
}
