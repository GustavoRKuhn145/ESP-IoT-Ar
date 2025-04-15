#include "ESP32Wifi.h"
#include "config.h"

void WifiInit()
{
    WiFi.mode(WIFI_STA);
    Serial.println("\nIniciando Conexão WiFi");
    bool connected = false;

    for(int i = 0; i < NUM_NETWORKS && !connected; i++) {
        Serial.print("Tentando conectar em: ");
        Serial.println(ssids[i]);

        WiFi.begin(ssids[i], passwords[i]);
        unsigned long startAttemptTime = millis();
        unsigned long lastDotPrintTime = 0;

        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS) {
            if (millis() - lastDotPrintTime >= 500) {
                Serial.print(".");
                lastDotPrintTime = millis();
            }
        }

        if (WiFi.status() == WL_CONNECTED) {
            connected = true;
            Serial.println("\nWiFi conectado");
            Serial.print("SSID: ");
            Serial.println(ssids[i]);
            Serial.print("Endereço IP: ");
            Serial.println(WiFi.localIP());
        }
        else {
            Serial.println("\nFalha ao conectar nessa rede");
        }
    }
    if (!connected) {
        Serial.println("Não foi possível conectar a nenhuma rede");
    }
}