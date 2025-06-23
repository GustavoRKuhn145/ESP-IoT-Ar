#ifndef CONTROLE_IR_UTILS_H
#define CONTROLE_IR_UTILS_H

// Definições
#define CADASTRO_DELAY_MS 15000

// Includes
#include "pins.h"

#include <IRremote.hpp>
#include <stdint.h>


// Struct para armazenar os dados de uma mensagem recebida
struct storedIRDataStruct {
    IRData receivedIRData; 
    uint8_t rawCode[RAW_BUFFER_LENGTH]; // The durations if raw
    uint8_t rawCodeLength; // The length of the code
} sStoredIRData;

void irInit();

bool storeCode();

void sendCode(storedIRDataStruct *aIRDataToSend);

#endif