#include "ESPNOW_common.h"
#include "ControleIrUtils.h"
#include "ESP32Wifi.h"
#include "config.h"
#include "pins.h"
#include "FileSystem.h"
#include "DBUtils.h"
#include "MqttUtils.h"
#include "globals.h"

#include <Arduino.h>
#include <IRutils.h>

#define IR_DEBOUCE_MS 300
unsigned long lastIrReceiveTime = 0;

void setup() {
    Serial.begin(115200);

    fsInit();

    WifiInit();
    
    DBInit();

    mqttInit();

    espNowControleInit(false);

    printMacAddress();

    irInit();

}
PowerData receivedData;
bool isDataReceived = false;
unsigned long currentMillis = millis();

int mode = 0; // 0 = modo normal: monitora o consumo; 1 = modo de envio: envia o comando de desligar; 2 = modo de cadastro: ativa o protocolo de cadastro
unsigned long waitStart = 0;

void loop() {

  currentMillis = millis();

  mqttLoop();

  // Loop principal de operação
  switch (mode) { 

    case 0: // modo normal de operação


    if (Serial.available()) {
      String commandLine = Serial.readStringUntil('\n');
      commandLine.trim(); // Remove leading/trailing whitespace

      if (commandLine.equalsIgnoreCase("list")) {
        listIRCommands();
      } else if (commandLine.startsWith("delete ")) {
        // Extract the index from the command
        String indexStr = commandLine.substring(7); // "delete " is 7 characters
        int indexToDelete = indexStr.toInt();
        if (indexStr.length() > 0 && indexToDelete >= 0) { // Basic validation
          deleteIRCommand(indexToDelete);
        } else {
          Serial.println("Invalid 'delete' command. Usage: 'delete <index>' (e.g., 'delete 0')");
        }
      } else if (commandLine.equalsIgnoreCase("clear")) {
        clearIRCommands();
      } else {
        Serial.println("Unknown command. Type 'record', 'play', 'list', 'delete <index>', or 'clear'.");
      }
    }



      // recebe e processar os dados do ESP-AR: funcionalidade feita pelo ESP-NOW
      if (isDataReceived) {
        sendPowerDataToInflux(receivedData);
        isDataReceived = false;
      }
      // TODO: espera o comando mqtt para trocar para o modo de desligar ou cadastrar

      break;
    
    case 1: // modo de desligamento
      //TODO: deligar todos os ares
      sendIRSignal();
      Serial.println("código enviado");
      mode = 0;
      break;
    
    case 2: // modo de cadastro
      currentMillis = millis();
      if (currentMillis - waitStart < 5000) {
        if(irrecv.decode(&results)) {
          Serial.print("Código IR Recebido! Tipo: ");
          Serial.println(typeToString(results.decode_type)); // Print decoded type

          // Save the received signal
          saveIRSignal(&results);

          // Resume receiving IR signals
          irrecv.resume();
          mode = 0;
          Serial.println("Código IR gravado.");
        }
      } else {
          Serial.println("Nenhuma leitura de código em 5s, retornando ao modo normal");
          mode = 0;
      }
      break;

    // TODO: case 4: broadcast de endereço MAC para parear novos ESP-AR
  }

  

}
