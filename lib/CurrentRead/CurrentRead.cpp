#include "CurrentRead.h"
#include "pins.h"

#include <esp_mac.h>

ACS712 ACS(ACS_PIN, 5, 4095, 106);

void ACSInit() {
    Serial.print("ACS712_LIB_VERSION: ");
    Serial.println(ACS712_LIB_VERSION);
    Serial.print("MidPoint: ");
    Serial.println(ACS.autoMidPoint(60, 10));
    Serial.print("Noise mV: ");
    Serial.println(ACS.getNoisemV());

    esp_err_t ret = esp_read_mac(powerData.senderMac, ESP_MAC_WIFI_STA);


    if (ret == ESP_OK) {
        Serial.print("MAC Address from EFUSE: ");
        for (int i = 0; i < 6; i++) {
        Serial.printf("%02X%s", powerData.senderMac[i], (i < 5 ? ":" : ""));
        }
        Serial.println();
    } else {
        Serial.printf("Failed to get MAC address from EFUSE: %s\n", esp_err_to_name(ret));
        // Common error: ESP_ERR_MAC_NOT_INIT if some part of the MAC system isn't ready.
        // Try esp_wifi_init() and then esp_wifi_get_mac() if this persists.
    }
}

float getCurrentReading() {
    float soma = 0;
    for (int i = 0; i < 60; i++) {
        soma += ACS.mA_AC(60);
    }

    float media_mA = soma / 60.0;
    float corrente_A = (media_mA-16) / 1000.0;

    return corrente_A;
}