#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>

void setupWebServer();

// Broadcast an app-state payload to all connected WebSocket clients.
void notifyClients(const String &payload);

// Keep WebSocket client state tidy (disconnect cleanup).
void cleanupClients();

// Number of active WebSocket clients.
size_t getConnectedClients();

#endif // WEB_SERVER_H
