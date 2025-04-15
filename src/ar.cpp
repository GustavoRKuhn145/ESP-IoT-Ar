#include <Arduino.h>

// Minha Bibliotecas
#include "OTACustom.h"
#include "ESP32Wifi.h"
#include "config.h"

void setup() {

  Serial.begin(115200);

  WifiInit();

  OTAInit(OTA_HOSTNAME_AR);

  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {

  ArduinoOTA.handle();

  delay(500);
  digitalWrite(LED_BUILTIN,HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN,LOW);
  
}
