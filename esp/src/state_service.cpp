#include "state_service.h"
#include "json_codec.h"
#include "led_runtime.h"
#include <Arduino.h>

static AppState gState;

static uint16_t clampGlobalLedCount(uint16_t ledCount) {
    return ledCount > GLOBAL_LED_LIMIT ? GLOBAL_LED_LIMIT : ledCount;
}

void stateServiceInitDefaults() {
    gState.led.mode = LedMode::Rainbow;
    gState.led.brightnessPercent = 30;
    gState.led.speedPercent = 75;
    gState.led.animationSpeed = 100;
    gState.led.solidColor = {.red = 15, .green = 128, .blue = 100};

    gState.wifi.ssid = "";
    gState.wifi.ip = "0.0.0.0";
    gState.wifi.rssi = 0;

    gState.network.dhcpEnabled = true;
    gState.network.ip = "0.0.0.0";
    gState.network.subnet = "255.255.255.0";

    gState.system.ledCount = 60;
}

const AppState &stateServiceGet() { return gState; }

// LED
void updateLedMode(LedMode mode) { gState.led.mode = mode; }
void updateLedBrightness(uint8_t brightness) {
    gState.led.brightnessPercent = brightness > 100 ? 100 : brightness;
}
void updateLedSpeed(uint16_t speed) {
    gState.led.speedPercent = speed > 100 ? 100 : speed;
}
void updateLedSolidColor(uint8_t red, uint8_t green, uint8_t blue) {
    gState.led.solidColor.red = red;
    gState.led.solidColor.green = green;
    gState.led.solidColor.blue = blue;
}
void updateSystemLedCount(uint16_t ledCount) {
    gState.system.ledCount = clampGlobalLedCount(ledCount);
}
