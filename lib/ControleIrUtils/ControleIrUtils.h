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

struct IRCommandData {
    unsigned long long timestamp; // Millis() or NTP timestamp
    decode_type_t decode_type;       // Enum from IRremoteESP8266
    uint64_t value;               // The decoded value (hex code)
    uint16_t bits;                // Number of bits
    uint32_t address;             // For protocols with address (e.g., NEC)
    uint32_t command;             // For protocols with command (e.g., NEC)
    bool repeat;                  // Is it a repeat code?
    // Could also include raw data here, but it's complex for LittleFS
    // uint16_t *rawbuf;
    // uint16_t rawlen;

    // Helper to convert protocol enum to string
    String getProtocolString() const;
    // Helper to convert struct to delimited string for LittleFS
    String toStringForFS() const;
    // Helper to convert delimited string from FS back to struct
    static IRCommandData fromStringForFS(const String& dataString);
};

void irInit();

bool readIRCommand(IRCommandData &commandData);
void sendIRCommand(const IRCommandData &commandData, int repeats = 0);

#endif