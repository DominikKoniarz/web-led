#include "led_runtime.h"

#include "state_service.h"
#include <Arduino.h>
#include <FastLED.h>
#include <cstring>

namespace {

constexpr uint8_t DATA_PIN = 8;
constexpr EOrder COLOR_ORDER = GRB;

CRGB gLeds[GLOBAL_LED_LIMIT];
uint8_t gHeat[GLOBAL_LED_LIMIT];

LedMode gLastMode = LedMode::Off;
uint16_t gChaseHead = 0;

unsigned long gLastRainbowStepMs = 0;
unsigned long gLastChaseStepMs = 0;
unsigned long gLastSparkleStepMs = 0;
unsigned long gLastFireStepMs = 0;
unsigned long gLastWaveStepMs = 0;

uint8_t gRainbowHue = 0;
uint8_t gWavePhase = 0;

uint16_t clampActiveLedCount(uint16_t requested) {
    return requested > GLOBAL_LED_LIMIT ? GLOBAL_LED_LIMIT : requested;
}

uint8_t clampPercent(uint8_t value) {
    if (value > 100) {
        return 100;
    }
    return value;
}
uint8_t clampPercent(uint16_t value) {
    if (value > 100) {
        return 100;
    }
    return static_cast<uint8_t>(value);
}

uint8_t toFastLEDBrightness(uint8_t brightnessPercent) {
    const uint8_t bounded = clampPercent(brightnessPercent);
    return static_cast<uint8_t>(map(bounded, 0, 100, 0, 255));
}

uint16_t speedToIntervalMs(uint16_t speedPercent, uint16_t slowMs,
                           uint16_t fastMs) {
    const uint8_t bounded = clampPercent(speedPercent);
    return static_cast<uint16_t>(map(bounded, 0, 100, slowMs, fastMs));
}

void clearUnusedLeds(uint16_t activeCount) {
    if (activeCount >= GLOBAL_LED_LIMIT) {
        return;
    }
    fill_solid(gLeds + activeCount, GLOBAL_LED_LIMIT - activeCount,
               CRGB::Black);
}

void resetAnimationState() {
    gChaseHead = 0;
    gRainbowHue = 0;
    gWavePhase = 0;
    gLastRainbowStepMs = 0;
    gLastChaseStepMs = 0;
    gLastSparkleStepMs = 0;
    gLastFireStepMs = 0;
    gLastWaveStepMs = 0;
    memset(gHeat, 0, sizeof(gHeat));
    fill_solid(gLeds, GLOBAL_LED_LIMIT, CRGB::Black);
}

void renderSolid(uint16_t activeCount, CRGB color) {
    fill_solid(gLeds, activeCount, color);
}

void renderRainbow(uint16_t activeCount, uint16_t speedPercent,
                   unsigned long nowMs) {
    const uint16_t stepIntervalMs = speedToIntervalMs(speedPercent, 150, 8);
    if (gLastRainbowStepMs == 0 ||
        (nowMs - gLastRainbowStepMs) >= stepIntervalMs) {
        gLastRainbowStepMs = nowMs;
        gRainbowHue++;
    }

    const uint8_t hueStep =
        activeCount > 0 ? static_cast<uint8_t>(255 / activeCount) : 0;
    fill_rainbow(gLeds, activeCount, gRainbowHue, hueStep == 0 ? 1 : hueStep);
}

void renderBreathing(uint16_t activeCount, uint16_t speedPercent,
                     CRGB baseColor) {
    const uint8_t bpm =
        static_cast<uint8_t>(map(clampPercent(speedPercent), 0, 100, 4, 28));
    const uint8_t breathe = beatsin8(bpm, 8, 255);

    fill_solid(gLeds, activeCount, baseColor);
    nscale8_video(gLeds, activeCount, breathe);
}

void renderChase(uint16_t activeCount, uint16_t speedPercent,
                 unsigned long nowMs, CRGB color) {
    const uint16_t stepIntervalMs = speedToIntervalMs(speedPercent, 220, 20);
    if (gLastChaseStepMs == 0 || (nowMs - gLastChaseStepMs) >= stepIntervalMs) {
        gLastChaseStepMs = nowMs;
        gChaseHead = activeCount > 0 ? (gChaseHead + 1) % activeCount : 0;
    }

    fill_solid(gLeds, activeCount, CRGB::Black);

    for (uint8_t tail = 0; tail < 5; ++tail) {
        const uint16_t idx =
            activeCount > 0 ? (gChaseHead + activeCount - tail) % activeCount
                            : 0;
        CRGB pixel = color;
        const uint8_t scale = static_cast<uint8_t>(255 - (tail * 48));
        pixel.nscale8_video(scale);
        gLeds[idx] = pixel;
    }
}

void renderSparkle(uint16_t activeCount, uint16_t speedPercent,
                   unsigned long nowMs, CRGB color) {
    const uint16_t stepIntervalMs = speedToIntervalMs(speedPercent, 160, 20);
    const uint8_t fadeAmount =
        static_cast<uint8_t>(map(clampPercent(speedPercent), 0, 100, 20, 70));

    if (gLastSparkleStepMs == 0 ||
        (nowMs - gLastSparkleStepMs) >= stepIntervalMs) {
        gLastSparkleStepMs = nowMs;

        fadeToBlackBy(gLeds, activeCount, fadeAmount);

        const uint8_t sparkCount =
            static_cast<uint8_t>(map(clampPercent(speedPercent), 0, 100, 1, 6));
        for (uint8_t i = 0; i < sparkCount; ++i) {
            if (activeCount == 0) {
                break;
            }
            const uint16_t idx = static_cast<uint16_t>(random16(activeCount));
            gLeds[idx] = color;
        }
    }
}

void renderFire(uint16_t activeCount, uint16_t speedPercent,
                unsigned long nowMs) {
    const uint16_t stepIntervalMs = speedToIntervalMs(speedPercent, 130, 18);
    if (!(gLastFireStepMs == 0 ||
          (nowMs - gLastFireStepMs) >= stepIntervalMs)) {
        return;
    }
    gLastFireStepMs = nowMs;

    const uint8_t cooling =
        static_cast<uint8_t>(map(clampPercent(speedPercent), 0, 100, 90, 40));
    const uint8_t sparking =
        static_cast<uint8_t>(map(clampPercent(speedPercent), 0, 100, 80, 180));

    for (uint16_t i = 0; i < activeCount; i++) {
        const uint8_t cooldown =
            static_cast<uint8_t>(random8(((cooling * 10) / activeCount) + 2));
        gHeat[i] = qsub8(gHeat[i], cooldown);
    }

    for (int i = activeCount - 1; i >= 2; i--) {
        gHeat[i] = static_cast<uint8_t>(
            (gHeat[i - 1] + gHeat[i - 2] + gHeat[i - 2]) / 3);
    }

    if (random8() < sparking) {
        const uint8_t y = random8(activeCount > 7 ? 7 : activeCount);
        gHeat[y] = qadd8(gHeat[y], random8(160, 255));
    }

    for (uint16_t j = 0; j < activeCount; j++) {
        gLeds[j] = HeatColor(gHeat[j]);
    }
}

void renderWave(uint16_t activeCount, uint16_t speedPercent,
                unsigned long nowMs, CRGB baseColor) {
    const uint16_t stepIntervalMs = speedToIntervalMs(speedPercent, 140, 10);
    if (gLastWaveStepMs == 0 || (nowMs - gLastWaveStepMs) >= stepIntervalMs) {
        gLastWaveStepMs = nowMs;
        gWavePhase += 3;
    }

    const uint8_t spatialStep =
        activeCount > 0 ? static_cast<uint8_t>(255 / activeCount) : 0;

    for (uint16_t i = 0; i < activeCount; i++) {
        CRGB c = baseColor;
        const uint8_t wave =
            sin8(gWavePhase + static_cast<uint8_t>(i * spatialStep));
        c.nscale8_video(wave);
        gLeds[i] = c;
    }
}

} // namespace

void ledRuntimeInit() {
    FastLED.addLeds<WS2812B, DATA_PIN, COLOR_ORDER>(gLeds, GLOBAL_LED_LIMIT);
    FastLED.setCorrection(TypicalLEDStrip);
    resetAnimationState();
    FastLED.show();
}

void ledRuntimeTick() {
    const AppState &state = stateServiceGet();
    const uint16_t activeCount = clampActiveLedCount(state.system.ledCount);
    const CRGB solidColor(state.led.solidColor.red, state.led.solidColor.green,
                          state.led.solidColor.blue);
    const unsigned long nowMs = millis();

    FastLED.setBrightness(toFastLEDBrightness(state.led.brightnessPercent));

    if (state.led.mode != gLastMode) {
        gLastMode = state.led.mode;
        resetAnimationState();
    }

    if (activeCount == 0 || state.led.mode == LedMode::Off) {
        fill_solid(gLeds, GLOBAL_LED_LIMIT, CRGB::Black);
        FastLED.show();
        return;
    }

    switch (state.led.mode) {
    case LedMode::Solid:
        renderSolid(activeCount, solidColor);
        break;
    case LedMode::Rainbow:
        renderRainbow(activeCount, state.led.speedPercent, nowMs);
        break;
    case LedMode::Breathing:
        renderBreathing(activeCount, state.led.speedPercent, solidColor);
        break;
    case LedMode::Chase:
        renderChase(activeCount, state.led.speedPercent, nowMs, solidColor);
        break;
    case LedMode::Sparkle:
        renderSparkle(activeCount, state.led.speedPercent, nowMs, solidColor);
        break;
    case LedMode::Fire:
        renderFire(activeCount, state.led.speedPercent, nowMs);
        break;
    case LedMode::Wave:
        renderWave(activeCount, state.led.speedPercent, nowMs, solidColor);
        break;
    case LedMode::Off:
        fill_solid(gLeds, activeCount, CRGB::Black);
        break;
    }

    clearUnusedLeds(activeCount);
    FastLED.show();
}