#include "pins.h"
#include "ControleIrUtils.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

int DELAY_BETWEEN_REPEAT = 50;

IRrecv irrecv(IR_RECV_PIN, MAX_RAW_LEN, 50, false);
IRsend irsend(IR_SEND_PIN);
decode_results results;

void irInit()
{
    irrecv.enableIRIn();
    irrecv.setUnknownThreshold(12);

    irsend.begin();
}

void saveIRSignal(const decode_results* res) {

    JsonDocument doc;

    File file = LittleFS.open(IR_DATA_FILE, "r");
    if (file) {
        DeserializationError error = deserializeJson(doc, file);
        if (error) {
            Serial.print("Failed to read existing IR data, starting fresh: ");
            Serial.println(error.c_str());
        }
        file.close();
    }

    JsonArray irCommandsArray;

    if (doc.is<JsonArray>()) {
        // If the document is already an array, get a reference to it
        irCommandsArray = doc.as<JsonArray>();
    } else if (doc.is<JsonObject>()) {
        // If the document is a single object (from a previous single-command save),
        // convert it into an array containing that object, then add the new one.
        JsonObject oldCommand = doc.as<JsonObject>();
        doc.clear();
        irCommandsArray = doc.to<JsonArray>();
        irCommandsArray.add(oldCommand);
        Serial.println("Converted previous single command to array format.");
    } else {
        // If it's neither an object nor an array (e.g., empty or corrupted), make it an empty array
        irCommandsArray = doc.to<JsonArray>();
    }

    JsonObject newSignal = irCommandsArray.add<JsonObject>();

    uint16_t correctedRawLen = getCorrectedRawLength(res);
    uint16_t* tempRawData = resultToRawArray(res);

      // Store the common IR data
    newSignal["value"] = res->value;
    newSignal["bits"] = res->bits;
    newSignal["decode_type"] = res->decode_type;
    newSignal["rawlen"] = correctedRawLen;
    newSignal["overflow"] = res->overflow;

    // Store the raw timing data (pulse/space pairs)
    JsonArray rawDataArray = newSignal["rawBuf"].to<JsonArray>();
    for (uint16_t i = 0; i < correctedRawLen; i++) {
        rawDataArray.add(tempRawData[i]);
    }

    delete[] tempRawData;

    // Open the file in write mode
    file = LittleFS.open(IR_DATA_FILE, "w");
    if (!file) {
        Serial.println("Failed to open file for writing IR data.");
        return;
    }

    // Serialize JSON to file
    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write IR data to file.");
    } else {
        Serial.println("IR signal saved successfully to LittleFS! Total commands: ");
        Serial.println(irCommandsArray.size());

        // Print the saved data for verification (optional)
        // serializeJsonPretty(doc, Serial);
    }

    file.close();
}

void sendIRSignal() {
    // Open the file in read mode
    File file = LittleFS.open(IR_DATA_FILE, "r");
    if (!file) {
        Serial.println("Failed to open file for reading IR data. Have you recorded a signal?");
        return;
    }

    JsonDocument doc;

    // Deserialize JSON from file
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        Serial.print("Failed to read file, using default configuration: ");
        Serial.println(error.c_str());
        file.close();
        return;
    }

    file.close();

    JsonArray irCommandsArray = doc.as<JsonArray>();
    if (irCommandsArray.isNull() || irCommandsArray.size() == 0) {
        Serial.println("No IR commands found in file.");
    }

     Serial.print("Playing ");
    Serial.print(irCommandsArray.size());
    Serial.println(" stored IR commands...");

    for (JsonObject signal : irCommandsArray) {
    uint64_t value = signal["value"];
    uint16_t bits = signal["bits"];
    decode_type_t decode_type = (decode_type_t)signal["decode_type"].as<int>();
    uint16_t rawlen = signal["rawlen"];
    bool overflow = signal["overflow"];

    JsonArray rawDataArray = signal["rawBuf"];
    if (rawDataArray.isNull() || rawlen != rawDataArray.size()) {
      Serial.println("Error: Mismatch between rawlen and rawBuf array size for a command. Skipping.");
      continue; // Skip this command if data is corrupted
    }

    // Allocate memory for rawBuf and copy data
    uint16_t* rawData = new uint16_t[rawlen];
    for (uint16_t i = 0; i < rawlen; i++) {
      rawData[i] = rawDataArray[i].as<uint16_t>();
    }

    Serial.print("  Playing command (Type: ");
    Serial.print(typeToString(decode_type)); // Convert decode_type_t to string for printing
    Serial.print(", RawLen: ");
    Serial.print(rawlen);
    Serial.println(")...");

    // For raw signals, use sendRaw. This is the most versatile for copying.
    // The frequency is typically 38kHz for most IR protocols.
    // IMPORTANT: If your AC units don't react, try changing 38 to 36, 40, or other values.
    irsend.sendRaw(rawData, rawlen, 38);

    // Clean up dynamically allocated memory
    delete[] rawData;

    // Add a small delay between sending commands if playing multiple,
    // to give the AC unit time to process each signal.
    delay(500); // 500ms delay
  }
  Serial.println("All stored IR signals sent!");
}






// Function to list all stored IR commands
void listIRCommands() {
  File file = LittleFS.open(IR_DATA_FILE, "r");
  if (!file) {
    Serial.println("No IR commands stored yet.");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.print("Failed to read IR data from file: ");
    Serial.println(error.c_str());
    file.close();
    return;
  }
  file.close();

  JsonArray irCommandsArray = doc.as<JsonArray>();
  if (irCommandsArray.isNull() || irCommandsArray.size() == 0) {
    Serial.println("No IR commands found in file.");
    return;
  }

  Serial.println("--- Stored IR Commands ---");
  int index = 0;
  for (JsonObject signal : irCommandsArray) {
    Serial.print("  [");
    Serial.print(index++);
    Serial.print("] Type: ");
    Serial.print(typeToString((decode_type_t)signal["decode_type"].as<int>()));
    Serial.print(", RawLen: ");
    Serial.println(signal["rawlen"].as<uint16_t>());

    // Print rawBuf data for detailed inspection
    JsonArray rawDataArray = signal["rawBuf"];
    if (!rawDataArray.isNull()) {
      Serial.print("    Raw Data (microseconds): ");
      for (uint16_t i = 0; i < rawDataArray.size(); i++) {
        Serial.print(rawDataArray[i].as<uint16_t>());
        if (i < rawDataArray.size() - 1) {
          Serial.print(", ");
        }
      }
      Serial.println();
      Serial.println("    (Look for large numbers (e.g., >10000) indicating pauses between sub-signals.)");
    }
  }
  Serial.println("--------------------------");
}

// Function to delete a specific IR command by index
void deleteIRCommand(int indexToDelete) {
  File file = LittleFS.open(IR_DATA_FILE, "r");
  if (!file) {
    Serial.println("No IR commands stored to delete.");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.print("Failed to read IR data for deletion: ");
    Serial.println(error.c_str());
    file.close();
    return;
  }
  file.close();

  JsonArray irCommandsArray = doc.as<JsonArray>();
  if (irCommandsArray.isNull() || indexToDelete < 0 || indexToDelete >= irCommandsArray.size()) {
    Serial.print("Invalid index for deletion: ");
    Serial.println(indexToDelete);
    Serial.print("Current number of commands: ");
    Serial.println(irCommandsArray.size());
    return;
  }

  irCommandsArray.remove(indexToDelete); // Remove the element at the specified index

  // Rewrite the file with the updated (removed) data
  file = LittleFS.open(IR_DATA_FILE, "w");
  if (!file) {
    Serial.println("Failed to open file for writing after deletion.");
    return;
  }

  if (serializeJson(doc, file) == 0) {
    Serial.println("Failed to save IR data after deletion.");
  } else {
    Serial.print("Command at index ");
    Serial.print(indexToDelete);
    Serial.print(" deleted successfully. Total commands remaining: ");
    Serial.println(irCommandsArray.size());
  }
  file.close();
}

// Function to clear all stored IR commands
void clearIRCommands() {
  if (LittleFS.remove(IR_DATA_FILE)) {
    Serial.println("All stored IR commands cleared from LittleFS.");
  } else {
    Serial.println("Failed to clear IR commands or file did not exist.");
  }
}