#include "ESPNOWcomm.h"
#include "OTAMonitor.h"
#include "config.h"

#include <esp_now.h>
#include <WebSerial.h>
#include <WiFi.h>
#include <esp_wifi.h>

// ---- Funções comuns ------------------------- //

void printMacAddress()
{
    uint8_t baseMac[6];
    esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
    if (ret == ESP_OK)
    {
        Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                      baseMac[0], baseMac[1], baseMac[2],
                      baseMac[3], baseMac[4], baseMac[5]);
    }
    else
    {
        Serial.println("Falha ao ler MAC address do ESP");
    }
}

void setupPeer(const uint8_t *peerMac, int wifiChannel, bool doEncrypt)
{
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peerMac, 6);
    peerInfo.channel = wifiChannel; // Use current channel
    peerInfo.encrypt = doEncrypt;
    esp_now_add_peer(&peerInfo);
}

bool didReceiveChannel = false;
int receivedChannel = 1;

// --------------------------------------------- //

// --- Funções do módulo ar -------------------- //

void arOnDataRecv(uint8_t *controleMac, const uint8_t *incomingData, int len)
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
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW falhou inicialização");
        return;
    }

    esp_now_register_recv_cb(arOnDataRecv);

    Serial.println("Aguardando canal WiFi do módulo do controle");
    unsigned long startTime = millis();
    while (!didReceiveChannel && millis() - startTime < 10000)
    {
        Serial.print(".");
        delay(100);
    }

    esp_now_deinit(); // reinicializa com novo canal
    delay(100);

    if (didReceiveChannel)
    {
        Serial.printf("\nCanal recebido, setando canal para: %d\n", receivedChannel);
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_channel(receivedChannel, WIFI_SECOND_CHAN_NONE);
        esp_wifi_set_promiscuous(false);
    }
    else
    {
        Serial.println("Canal não recebido a tempo, usando padrão");
    }

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW falhou reinicialização com novo canal");
        return;
    }

    setupPeer(controleMac, receivedChannel, doEncrypt);

    Serial.println("Conexão esp-now concluída");
}

// --------------------------------------------- //

// --- Funções do módulo controle -------------- //

void controleOnDataRecv(const uint8_t *arMac, uint8_t *incomingData, int len)
{
    if (len == sizeof(PowerDraw))
    {
        PowerDraw receivedData;
        memcpy(&receivedData, incomingData, sizeof(receivedData));
        Serial.printf("Potência: %dW, Current: %dmA\n", receivedData.power, receivedData.current);

        // TODO: enviar para influxDB aqui
    }
    else
    {
        Serial.printf("Tamanho de mensagem não esperado: %d\n", len);
    }
}

void espNowControleInit(bool doEncrypt)
{

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Falha ao conectar ao wifi");
        return;
    }
    int wifiChannel = WiFi.channel();
    Serial.printf("Conectado ao WiFi no canal: %d\n", wifiChannel);

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW falhou inicialização");
        return;
    }

    setupPeer(arMac, doEncrypt = doEncrypt);

    uint8_t data[] = {static_cast<uint8_t>(wifiChannel)};
    esp_err_t result = esp_now_send(arMac, data, sizeof(data));
    if (result == ESP_OK)
    {
        Serial.println("Channel info sent to ESP#1");
    }
    else
    {
        Serial.println("Failed to send channel info");
    }

    esp_now_register_recv_cb(controleOnDataRecv);
}
// --------------------------------------------- //