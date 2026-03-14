#include <Arduino.h>

#define LED_PIN 15

int sum(int a, int b) { return a + b; }

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(1000);

    auto result = sum(5, 10);

    Serial.println("5 + 10 is: " + String(result));
}
