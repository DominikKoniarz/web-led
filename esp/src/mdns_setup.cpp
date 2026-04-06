#include "mdns_setup.h"
#include <Arduino.h>
#include <ESPmDNS.h>

constexpr const char *MDNS_HOSTNAME = "webled";

void setupMdns() {
    if (!MDNS.begin(MDNS_HOSTNAME)) {
        Serial.println("[mDNS] Initialization failed");
    } else {
        Serial.printf("[mDNS] Ready: http://%s.local\n", MDNS_HOSTNAME);
    }
}
