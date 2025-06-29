#ifndef CURRENT_READ_H
#define CURRENT_READ_H

#include <ACS712.h>

extern ACS712 ACS;

extern struct PowerData {
    uint8_t senderMac[6];
    float currentReading;
    unsigned long timeSinceLastRead_ms;
    unsigned long timestamp_ms;
} powerData;

void ACSInit();

float getCurrentReading();

#endif