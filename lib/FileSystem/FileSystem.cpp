#include "FileSystem.h"
#include "ControleIrUtils.h"

void fsInit() {
    if (!LittleFS.begin()) {
        Serial.println(F("Sistema de arquivos falhou ao iniciar!"));
        return;
    }
}

bool saveIRDataToFile(const char* filename, storedIRDataStruct &entry) {
    File file = LittleFS.open(filename, "a");
    if (!file) return false;

    file.write((uint8_t*)&entry, sizeof(entry));
    file.close();
    Serial.println(F("Comando IR salvo"));
    return true;
}