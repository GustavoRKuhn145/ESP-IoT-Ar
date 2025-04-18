#include "OTAMonitor.h"

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <WebSerial.h>

AsyncWebServer server(80);

void OTAMonitorInit() {
      // Serve a simple webpage
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! This is WebSerial demo. You can access the WebSerial interface at http://" + WiFi.localIP().toString() + "/webserial");
  });

  // Initialize WebSerial
  WebSerial.begin(&server);

  // Attach a callback function to handle incoming messages
  WebSerial.onMessage([&](uint8_t *data, size_t len) {
    Serial.printf("Received %lu bytes from WebSerial: ", len);
    Serial.write(data, len);
    Serial.println();
    WebSerial.println("Received Data...");
    String d = "";
    for(size_t i = 0; i < len; i++){
      d += char(data[i]);
    }
    WebSerial.println(d);
  });

  // Start the server
  server.begin();
};

