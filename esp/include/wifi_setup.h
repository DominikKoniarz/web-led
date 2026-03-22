#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <Arduino.h>

void setupWiFi();
void wifiSetupTick();

bool wifiStartProvisioningConnect(const String &ssid, const String &password,
                                  String &error);
String wifiScanNetworksJson();

#endif // WIFI_SETUP_H
