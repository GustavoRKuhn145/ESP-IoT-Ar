#ifndef MQTT_UTILS_H
#define MQTT_UTILS_H

#include "ESP32Wifi.h"
#include <PubSubClient.h>

PubSubClient mqttClient(*client);

void mqttConnect();

void mqttLoop();

void mqttCallback(char* topic, byte* payload, unsigned int length);

#endif