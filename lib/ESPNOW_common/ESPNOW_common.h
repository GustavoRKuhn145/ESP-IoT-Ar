#ifndef ESPNOW_COM_H
#define ESPNOW_COM_H

#include "config.h"

#include <esp_now.h>

typedef struct PowerDraw {
    int power;
    int current;
} PowerDraw;

// Inicializa o protocolo espnow para o módulo do controle, verificando o canal do wifi
// e enviando para o módulo do ar essas informações
void espNowControleInit(bool doEncrypt=false);

// Função de callback para ser chamada toda vez que o módulo do controle recebe informações do
// módulo do ar, isso inclui os dados no struct PowerDraw
void controleOnDataRecv(const uint8_t *arMac, const uint8_t *incomingData, int len);

// Inicializa o protocolo espnow para o módulo do ar, recebe as informações de canal wifi
// do módulo do controle e conecta no mesmo canal
void espNowArInit(bool doEncrypt=false);

// Função de callback para ser chamada toda vez que o módulo do ar recebe informações do módulo
// do controle, geralmente apenas o canal do wifi
void arOnDataRecv(const uint8_t *controleMac, const uint8_t *incomingData, int len);

void setupPeer(const uint8_t *peerMacAddress, bool doEncrypt=false);

void printMacAddress();

int32_t getWiFiChannel(const char *ssids[]);

#endif