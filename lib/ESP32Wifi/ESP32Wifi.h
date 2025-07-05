#ifndef ESP32WIFI_H
#define ESP32WIFI_H
#include "globals.h"

#include <WiFi.h>

#define WIFI_TIMEOUT_MS 10000

extern WiFiClient client;

void WifiInit();

#endif