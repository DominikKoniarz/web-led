#include "LittleFS.h"
#include "state_service.h"
#include "web_server.h"
#include "wifi_setup.h"
#include <Arduino.h>

#define LED_PIN 15

static const unsigned long WIFI_CHECK_INTERVAL = 30000; // 30 seconds
static unsigned long lastWiFiCheck = 0;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.begin(115200);
    delay(1000);

    if (!LittleFS.begin(true)) {
        Serial.println("LittleFS mount failed");
        return;
    }

    stateServiceInitDefaults();

    setupWiFi();

    setupWebServer();

    // 3 blinks to indicate successful setup
    for (int i = 0; i < 3; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(100);
        digitalWrite(LED_PIN, LOW);
        delay(100);
    }
}

void loop() {
    delay(10);

    unsigned long now = millis();
    if (now - lastWiFiCheck >= WIFI_CHECK_INTERVAL) {
        lastWiFiCheck = now;
        reconnectWiFi();
    }
}
