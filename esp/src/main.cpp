#include "LittleFS.h"
#include "state_service.h"
#include "web_server.h"
#include "wifi_setup.h"
#include <Arduino.h>

#define LED_PIN 15

void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.begin(115200);
    delay(1500);

    if (!LittleFS.begin(true, "/littlefs", 10, "littlefs")) {
        Serial.println("LittleFS mount failed");
        return;
    }

    // This is probably temporary
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

constexpr unsigned long TEMP_LOG_INTERVAL_MS = 3000;

void loop() {
    static unsigned long lastTempLogMs = 0;

    delay(500);

    // wifiSetupTick();

    cleanupClients();

    const unsigned long now = millis();
    if (now - lastTempLogMs >= TEMP_LOG_INTERVAL_MS) {
        lastTempLogMs = now;
        Serial.printf("CPU temp: %.2f C\n", temperatureRead());
    }
}
