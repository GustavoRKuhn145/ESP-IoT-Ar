#include "ESPNOW_common.h"
#include "ControleIrUtils.h"
#include "ESP32Wifi.h"
#include "config.h"
#include "pins.h"
#include "FileSystem.h"
#include "DBUtils.h"
#include "MqttUtils.h"

#include <Arduino.h>

#define IR_DEBOUCE_MS 300
unsigned long lastIrReceiveTime = 0;

void setup() {
    Serial.begin(115200);

    fsInit();

    WifiInit();
    
    DBInit();

    mqttConnect();

    espNowControleInit(false);

    printMacAddress();

    irInit();

}

PowerData receivedData;
IRCommandData irCommand;
bool isDataReceived = false;
unsigned long currentMillis = millis();

static int mode = 0; // 0 = modo normal: monitora o consumo; 1 = modo de envio: envia o comando de desligar; 2 = modo de cadastro: ativa o protocolo de cadastro
static unsigned long waitStart = 0;

void loop() {

  currentMillis = millis();

  mqttLoop();

  // Loop principal de operação
  switch (mode) { 

    case 0: // modo normal de operação
      // recebe e processar os dados do ESP-AR: funcionalidade feita pelo ESP-NOW
      if (isDataReceived) {
        sendPowerDataToInflux(receivedData);
        isDataReceived = false;
      }
      // TODO: espera o comando mqtt para trocar para o modo de desligar ou cadastrar

      break;
    
    case 1: // modo de envio de sinal de desligar
      //TODO: deligar todos os ares
      sendIRCommand(irCommand);
      Serial.println("código enviado");
      mode = 0;
      break;
    
    case 2: // Espera o comando novo por 5s
      if (currentMillis - waitStart < 5000) {
        if(readIRCommand(irCommand)) {
          if (saveIRDataToFile("/ir_codes.bin", irCommand.toStringForFS())) {
            Serial.println(F("Código Salvo"));
            lastIrReceiveTime = currentMillis;
            mode = 0;
          } else {
            Serial.println(F("Falha ao salvar o código"));
          }
        }
        else {
          Serial.println("Nenhuma leitura de código em 5s, retornando ao modo normal");
        }
      }
      break;

    // TODO: case 4: broadcast de endereço MAC para parear novos ESP-AR
  }
}
