#include "wifi_setup.h"
#include "secrets.h"
#include <WiFi.h>

void setupWiFi() {
    WiFi.mode(WIFI_STA);
    // For env variables usage check SECRETS.md
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.println("WIFI_SSID: " + String(WIFI_SSID));
    Serial.println("WIFI_PASSWORD: " + String(WIFI_PASSWORD));

    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnectWiFi() {
    if (WiFi.status() == WL_CONNECTED) {
        return;
    }

    Serial.println("WiFi lost. Attempting reconnect...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.print("Reconnected! IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println();
        Serial.println("Reconnect failed. Will retry next loop.");
    }
}
