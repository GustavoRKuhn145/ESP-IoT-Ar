#include "ESPNOW_common.h"
#include "ControleIrUtils.h"
#include "ESP32Wifi.h"
#include "config.h"
#include "pins.h"
#include "FileSystem.h"

#include <Arduino.h>

void setup() {
    Serial.begin(115200);

    fsInit();

    WifiInit();

    espNowControleInit(false);

    printMacAddress();

    irInit();
}

unsigned long currentMillis = millis();
unsigned long waitStart = 0;
int mode = 0; // 0 = modo normal: monitora o consumo; 1 = modo de envio: envia o comando de desligar; 2 = modo de cadastro: ativa o protocolo de cadastro
String irCommand;

void loop() {
  switch (mode) {

    case 0: // modo normal de operação
      // TODO: recebe e processar os dados do ESP-AR
      // TODO: espera o comando para trocar de modo adicionar waitStart = currentMillis;
      if(readIRCommand(irCommand)) {
        if (saveIRDataToFile("/ir_codes.bin", irCommand)) {
          Serial.println(F("Código Salvo"));
          mode = 1;
          waitStart = currentMillis;
          break;
        } else {
          Serial.println(F("Falha ao salvar o código"));
        }
      }
      break;
    
    case 1: // modo de envio de sinal de desligar
      //TODO: deligar todos os ares
      if (currentMillis - waitStart < 5000) {
        //TODO
        delay(1000);
      } else {
        mode = 0;
        break;
      }
      break;
    
    case 2: // 
      currentMillis = millis();
      
      if (currentMillis - waitStart < CADASTRO_DELAY_MS) {
        if (readIRCommand(irCommand)) {
          mode = 0;
          Serial.println("Código cadastrado com sucesso!");
          break;
        }
      }
      mode = 0;
      Serial.println("Tempo de cadastro esgotado!");
      break;

    // TODO: case 4: broadcast de endereço MAC para parear novos ESP-AR
  }
}
