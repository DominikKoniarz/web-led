#include "LittleFS.h"
#include "led_runtime.h"
#include "mdns_setup.h"
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

    logLittleFsHttpContents();

    stateServiceInitDefaults();
    stateServiceLoadFromFs();
    ledRuntimeInit();

    setupWiFi();
    setupMdns();

    setupWebServer();
}

void loop() {
    delay(16); // run at ~60Hz

    stateServiceTick();
    ledRuntimeTick();
}
