#include "OTAUpload.h"
#include "config.h"

void OTAUploadInit(const char* hostname) {
    // Inicialização do OTA
  ArduinoOTA.setHostname(hostname);

  // (Opcional) Senha para autenticação OTA
  // ArduinoOTA.setPassword("minha_senha");

  // Eventos OTA (opcional para debug)
  ArduinoOTA.onStart([]() {
    Serial.println("Iniciando OTA...");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA finalizada!");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progresso: %u%%\r", (progress * 100) / total);
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Erro [%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Erro de autenticação");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Erro ao iniciar");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Erro de conexão");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Erro ao receber");
    else if (error == OTA_END_ERROR) Serial.println("Erro ao finalizar");
  });

  ArduinoOTA.begin();
  Serial.println("OTA pronto!");
}