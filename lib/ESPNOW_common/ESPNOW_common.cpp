#include "ESPNOW_common.h"
#include "config.h"

#include <esp_now.h>
#include <esp_wifi.h>
#include <HardwareSerial.h>
#include <WiFi.h>

// ---- Funções comuns ------------------------- //

int32_t getWiFiChannel(const char *ssids[])
{
    if (int32_t n = WiFi.scanNetworks())
    {
        for (size_t i = 0; i < NUM_NETWORKS; i++)
        {
            const char* ssid = ssids[i];
            Serial.println(ssid);
            for (uint8_t j = 0; j < n; j++)
            {
                if (!strcmp(ssid, WiFi.SSID(j).c_str()))
                {
                    return WiFi.channel(j);
                }
            }
        }
    }
    return 0;
}

void printMacAddress()
{
    uint8_t baseMac[6];
    esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
    if (ret == ESP_OK)
    {
        Serial.print("Endereço MAC: ");
        Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                      baseMac[0], baseMac[1], baseMac[2],
                      baseMac[3], baseMac[4], baseMac[5]);
    }
    else
    {
        Serial.println("Falha ao ler MAC address do ESP");
    }
}

void setupPeer(const uint8_t *peerMac, bool doEncrypt)
{
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peerMac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = doEncrypt;
    if (doEncrypt)
    {
        for (uint8_t i = 0; i < 16; i++)
        {
            peerInfo.lmk[i] = LMK_STR[i];
        }
    }
    if (!esp_now_is_peer_exist(peerMac))
    {
        if (esp_now_add_peer(&peerInfo) != ESP_OK)
        {
            Serial.println("Falha ao adicionar peerAddress");
        }
    }
}