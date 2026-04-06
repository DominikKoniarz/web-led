#include "LittleFS.h"
#include "led_runtime.h"
#include "state_service.h"
#include "web_server.h"
#include "wifi_setup.h"
#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    delay(1500);

    if (!LittleFS.begin(true, "/littlefs", 10, "littlefs")) {
        Serial.println("LittleFS mount failed");
        return;
    }

    // This is probably temporary
    stateServiceInitDefaults();
    ledRuntimeInit();

    setupWiFi();

    setupWebServer();
}

constexpr unsigned long TEMP_LOG_INTERVAL_MS = 3000;

void loop() {
    static unsigned long lastTempLogMs = 0;

    delay(16); // run at ~60Hz

    ledRuntimeTick();

    const unsigned long now = millis();
    if (now - lastTempLogMs >= TEMP_LOG_INTERVAL_MS) {
        lastTempLogMs = now;
        Serial.printf("CPU temp: %.2f C\n", temperatureRead());
    }
}
