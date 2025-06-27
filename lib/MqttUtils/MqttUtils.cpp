#include "config.h"
#include "MqttUtils.h"

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    
}

void mqttConnect() {
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);

    while (!mqttClient.connected()) {
        Serial.print("Conectando ao MQTT...");
        if (mqttClient.connect(ESP_NOME)){
            Serial.println("Conectado!");
            //TODO: conectar nos tópicos de controle e envio de dados
        }
        else {
            Serial.print("Conexão falhou: ");
            Serial.print(mqttClient.state());
            delay(3000);
        }
    }
}

void mqttLoop() {
    if (!mqttClient.connected()) mqttConnect();
    mqttClient.loop();
}