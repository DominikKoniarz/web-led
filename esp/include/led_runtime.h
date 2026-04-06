#ifndef LED_RUNTIME_H
#define LED_RUNTIME_H

#include <stdint.h>

constexpr uint16_t GLOBAL_LED_LIMIT = 120;

void ledRuntimeInit();
void ledRuntimeTick();

#endif // LED_RUNTIME_H