#ifndef ESP32WIFI_H
#define ESP32WIFI_H

#include <WiFi.h>

#define WIFI_TIMEOUT_MS 5000

extern WiFiClient* client;

void WifiInit();

#endif