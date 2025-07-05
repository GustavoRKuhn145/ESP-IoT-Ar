#ifndef MQTT_UTILS_H
#define MQTT_UTILS_H

#include "globals.h"

#include <PubSubClient.h>

static const char* modeTopic = "sala219/mode";

extern PubSubClient mqttClient;

void mqttInit();

void mqttReconnect();

void mqttLoop();

void mqttCallback(char* topic, byte* payload, unsigned int length);

#endif