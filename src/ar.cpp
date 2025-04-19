// Minha Bibliotecas
#include "OTAMonitor.h"
#include "OTAUpload.h"
#include "ESP32Wifi.h"
#include "ESPNOW_common.h"
#include "config.h"

#include <Arduino.h>
#include <Blinkenlight.h>
#include <ArduinoOTA.h>
#include <WebSerial.h>
#include <esp_wifi.h>

Blinkenlight led(BUILTIN_LED);

void setup()
{

  Serial.begin(115200);

  WifiInit();

  // OTAUploadInit(OTA_HOSTNAME_AR);

  // OTAMonitorInit();

  espNowArInit(false);

  // led.blink();

  printMacAddress();

}

void loop()
{

  // ArduinoOTA.handle();
  // WebSerial.loop();
  // led.update();

  // Example fake data, replace with ACS712 read
  unsigned long currentMillis = millis();
  static unsigned long lastSent = 0;

  if (currentMillis - lastSent >= 5000)
  {
    lastSent = currentMillis;

    PowerDraw data = {random(100, 500), random(100, 500)};
    esp_err_t result = esp_now_send(controleMac, (uint8_t *)&data, sizeof(data));
    Serial.println(result == ESP_OK ? "Sent queued" : "Send error");
  }

}
