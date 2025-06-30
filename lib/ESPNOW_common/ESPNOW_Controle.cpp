#include "ESPNOW_common.h"
#include "config.h"
#include "CurrentRead.h"
#include "DBUtils.h"

#include <esp_now.h>
#include <HardwareSerial.h>
#include <esp_wifi.h>
#include <WiFi.h>

// --- Funções do módulo controle -------------- //

void controleOnDataRecv(const uint8_t *arMac, const uint8_t *incomingData, int len)
{
    Serial.print("Recebi algo: ");
    if (len == sizeof(PowerData))
    {
        memcpy(&receivedData, incomingData, sizeof(PowerData));

        Serial.printf("Leitura de Corrente: %.2fmA, Tempo desde última leitura: %lumS\n", receivedData.currentReading, receivedData.timeSinceLastRead_ms);

        isDataReceived = true;
    }
    else
    {       
        Serial.printf("Tamanho de mensagem não esperado: %d\n", len);
    }
}

void espNowControleInit(bool doEncrypt)
{
    esp_wifi_set_ps(WIFI_PS_NONE);

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW falhou inicialização");
        return;
    }

    esp_now_register_recv_cb(esp_now_recv_cb_t(controleOnDataRecv));
}
// --------------------------------------------- //