#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>

void setupWebServer();
void notifyClients(const String &payload);
void cleanupClients();
size_t getConnectedClients();

#endif // WEB_SERVER_H
