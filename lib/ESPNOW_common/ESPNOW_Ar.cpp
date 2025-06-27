#include "ESPNOW_common.h"
#include "config.h"

#include <esp_now.h>
#include <HardwareSerial.h>
#include <esp_wifi.h>
#include <WiFi.h>

// --------------------------------------------- //

// --- Funções do módulo ar -------------------- //


void espNowArInit(bool doEncrypt)
{

    WiFi.disconnect();

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW falhou inicialização");
        return;
    }

    setupPeer(controleMac, doEncrypt);

    esp_now_register_send_cb([](const uint8_t *mac, esp_now_send_status_t status)
                             { Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Send OK" : "Send Failed"); });
}
