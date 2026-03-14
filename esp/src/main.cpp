#include "LittleFS.h"
#include <Arduino.h>

#define LED_PIN 15

void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.begin(115200);
    delay(500);

    if (!LittleFS.begin(true)) {
        Serial.println("LittleFS mount failed");
        return;
    }
}

void loop() {
    // read all files in the root directory
    File root = LittleFS.open("/");
    while (true) {
        File file = root.openNextFile();
        if (!file) {
            break;
        }
        Serial.println(file.name());
        file.close();
    }
    root.close();

    delay(5000);
}
