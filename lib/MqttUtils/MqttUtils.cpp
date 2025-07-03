#include "config.h"
#include "MqttUtils.h"

extern int mode;
extern unsigned long waitStart;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Mensagem MQTT recebida: [");
    Serial.print(topic);
    Serial.print("} ");

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