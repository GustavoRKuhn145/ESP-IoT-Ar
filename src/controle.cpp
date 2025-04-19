#include "ESPNOW_common.h"
#include "ESP32Wifi.h"
#include "OTAMonitor.h"
#include "OTAUpload.h"
#include "config.h"

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <esp_wifi.h>



void setup() {
    Serial.begin(115200);

    WifiInit();

    // OTAUploadInit(OTA_HOSTNAME_CONTROLE);

    // OTAMonitorInit();

    espNowControleInit(false);

    printMacAddress();
}

void loop() {
    // ArduinoOTA.handle();
    unsigned long currentMillis = millis();
  static unsigned long lastSent = 0;

  if (currentMillis - lastSent >= 5000)
  {
    lastSent = currentMillis;

  }
}
