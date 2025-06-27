#include "FileSystem.h"


void fsInit() {
     // Try to begin, if it fails, try to format
    if (!LittleFS.begin()) {
        Serial.println(F("LittleFS: Attempting format..."));
        if (LittleFS.format()) {
            Serial.println(F("LittleFS: Format successful. Retrying mount..."));
            if (!LittleFS.begin()) {
                Serial.println(F("LittleFS: Mount failed after format!"));
            } else {
                Serial.println(F("LittleFS: Successfully mounted after format."));
            }
        } else {
            Serial.println(F("LittleFS: Format failed!"));
        }
    } else {
        Serial.println(F("LittleFS: Successfully mounted."));
    }
}

bool saveIRDataToFile(const char* filename, const String &data) {
    File file = LittleFS.open(filename, "a");
    if (!file) {
        Serial.println("Falha ao abrir o arquivo de códigos IR");
        return false;
    }
    if (file.print(data + "\n")) {
        Serial.println("Código adicionado!");
        file.close();
        return true;
    } else {
        Serial.println("Falha ao adicionar o Código!");
        file.close();
        return false;
    }
}

String readFile(const char* filename) {
    File file = LittleFS.open(filename, "r"); // "r" for read
    if (!file) {
        Serial.println("Failed to open file for reading");
        return "";
    }
    String fileContent = "";
    while (file.available()) {
        fileContent += (char)file.read();
    }
    file.close();
    return fileContent;
}

bool deleteFile(const char* filename) {
    if (LittleFS.remove(filename)) {
        Serial.println("File deleted successfully");
        return true;
    } else {
        Serial.println("File deletion failed");
        return false;
    }
}