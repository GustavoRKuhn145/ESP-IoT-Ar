#ifndef OTA_MONITOR_H
#define OTA_MONITOR_H

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <WebSerial.h>

extern AsyncWebServer server;

void OTAMonitorInit();

#endif
