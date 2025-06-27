#ifndef CONTROLE_IR_UTILS_H
#define CONTROLE_IR_UTILS_H

// Definições
#define CADASTRO_DELAY_MS 15000

// Includes
#include "pins.h"

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>

extern IRrecv irrecv;
extern IRsend irsend;
extern decode_results results;

void irInit();

bool readIRCommand(String &commandCode);
void sendIRCommand(const String &hexCode, int repeats = 0);

#endif