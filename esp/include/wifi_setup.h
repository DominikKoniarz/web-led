#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <Arduino.h>

void setupWiFi();
void reconnectWiFi();

// Phase 1 provisioning contract hook. Apply in-memory credentials only.
void setWiFiCredentials(const String &ssid, const String &password);

#endif // WIFI_SETUP_H
