#ifndef CONTROLE_IR_UTILS_H
#define CONTROLE_IR_UTILS_H

// Definições
#define CADASTRO_DELAY_MS 15000
#define IR_DATA_FILE "/ir_signal.json"
#define LONG_PAUSE_THRESHOLD_US 1000

// Includes
#include "pins.h"

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>

extern IRrecv irrecv;
extern IRsend irsend;
extern decode_results results;

const uint16_t MAX_RAW_LEN = 600;

void irInit();

void saveIRSignal(const decode_results* res);
void sendIRSignal();
void listIRCommands();
void deleteIRCommand(int indexToDelete);
void clearIRCommands();

#endif