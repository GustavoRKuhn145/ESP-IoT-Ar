#include "config.h"
#include "ESP32Wifi.h"
#include "MqttUtils.h"
#include "globals.h"

PubSubClient mqttClient(client);

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Mensagem MQTT recebida: [");
    Serial.print(topic);
    Serial.print("] ");

    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.println(message);

    if (String(topic) == modeTopic) {
        if (message == "desligar") {
            mode = 1; // Mudar para o modo de desligar
            Serial.println("Modo mudado para 1 (Desligar)");
        } else if (message == "cadastrar") {
            mode = 2; // Mudar para o modo de cadastro
            waitStart = millis(); // Start the 5s timer for mode 2
            Serial.println("Modo mudado para 2 (Cadastrar comando IR)");
        } else if (message == "normal") {
            mode = 0; // Change to normal mode
            Serial.println("Modo mudado para 0 (Normal)");
        }
    }
}

void mqttReconnect() {
     // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random mqttClient ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement and subscribe to topics
      mqttClient.publish("esp32/status", "hello from ESP32");
      mqttClient.subscribe(modeTopic); // Subscribe to the mode control topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqttLoop() {
    if (!mqttClient.connected()) mqttReconnect();
    mqttClient.loop();
}

void mqttInit() {
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
}