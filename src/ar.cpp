// Minha Bibliotecas
#include "OTAMonitor.h"
#include "OTAUpload.h"
#include "ESP32Wifi.h"
#include "config.h"

#include <Arduino.h>
#include <Blinkenlight.h>
#include <ArduinoOTA.h>
#include <WebSerial.h>

Blinkenlight led(BUILTIN_LED);

void setup() {

  Serial.begin(115200);

  WifiInit();

  OTAUploadInit(OTA_HOSTNAME_AR);

  OTAMonitorInit();

  led.blink();
}

void loop() {

  ArduinoOTA.handle();
  WebSerial.loop();
  led.update();

  static unsigned long last_print_time = millis();

  if ((unsigned long)(millis() - last_print_time) > 2000) {
    WebSerial.print(F("IP address: "));
    WebSerial.println(WiFi.localIP());
    WebSerial.printf("Uptime: %lums\n", millis());
    WebSerial.printf("Free heap: %u\n", ESP.getFreeHeap());
    last_print_time = millis();
  }


}
