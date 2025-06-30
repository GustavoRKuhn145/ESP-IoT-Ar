#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include "CurrentRead.h"

extern InfluxDBClient dbClient;

extern Point sensor;

void DBInit();

void sendCommandToInflux(String commandData);

void sendPowerDataToInflux(PowerData data);