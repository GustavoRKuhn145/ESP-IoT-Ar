// Minha Bibliotecas
#include "ESP32Wifi.h"
#include "ESPNOW_common.h"
#include "config.h"
#include "CurrentRead.h"

#include <Arduino.h>
#include <Blinkenlight.h>
#include <esp_wifi.h>

Blinkenlight led(BUILTIN_LED);

static unsigned long lastSent = 0;

void setup()
{

  Serial.begin(115200);

  WifiInit();

  espNowArInit(false);

  // led.blink();

  printMacAddress();

}

void loop()
{ 

  unsigned long currentMillis = millis();

  if (currentMillis - lastSent >= 5000)
  {
    
    powerData.currentReading = getCurrentReading();

    powerData.timestamp_ms = currentMillis;
    
    powerData.timeSinceLastRead_ms = currentMillis - lastSent;
    lastSent = currentMillis;
    

    esp_err_t result = esp_now_send(controleMac, (uint8_t *)&powerData, sizeof(powerData));
    Serial.println(result == ESP_OK ? "Sent queued" : "Send error");
  }
}
