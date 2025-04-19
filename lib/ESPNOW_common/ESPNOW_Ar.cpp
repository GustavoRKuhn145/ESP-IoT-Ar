#include "ESPNOW_common.h"
#include "config.h"

#include <esp_now.h>
#include <HardwareSerial.h>
#include <esp_wifi.h>
#include <WiFi.h>

bool didReceiveChannel = false;
int receivedChannel = 1;

// --------------------------------------------- //

// --- Funções do módulo ar -------------------- //

void arOnDataRecv(const uint8_t *controleMac, const uint8_t *incomingData, int len)
{
    if (len == 1)
    {
        receivedChannel = incomingData[0];
        didReceiveChannel = true;
        Serial.printf("Canal wifi recebido: %d\n", receivedChannel);
    }
}

void espNowArInit(bool doEncrypt)
{

    WiFi.disconnect();

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW falhou inicialização");
        return;
    }

    esp_now_register_recv_cb(arOnDataRecv);

    setupPeer(controleMac, doEncrypt);

    esp_now_register_send_cb([](const uint8_t *mac, esp_now_send_status_t status)
                             { Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Send OK" : "Send Failed"); });
}
