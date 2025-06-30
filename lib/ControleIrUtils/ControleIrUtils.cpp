#include "pins.h"
#include "ControleIrUtils.h"
#include <Arduino.h>

int DELAY_BETWEEN_REPEAT = 50;

IRrecv irrecv(IR_RECV_PIN);
IRsend irsend(IR_SEND_PIN);
decode_results results;

void irInit()
{
    irrecv.enableIRIn();
    irrecv.setUnknownThreshold(12);

    irsend.begin();
}

// Helper to convert decode_type enum to string
String IRCommandData::getProtocolString() const {
    switch (decode_type) {
        case NEC: return "NEC";
        case SONY: return "SONY";
        case RC5: return "RC5";
        case RC6: return "RC6";
        case PANASONIC: return "PANASONIC";
        case JVC: return "JVC";
        case SAMSUNG: return "SAMSUNG";
        case DENON: return "DENON";
        case PRONTO: return "PRONTO"; // Treat as raw for now
        case UNKNOWN: return "UNKNOWN";
        case RAW: return "RAW"; // Explicit raw capture
        default: return "UNHANDLED_PROTOCOL_TYPE";
    }
}

// Helper to convert struct to delimited string for LittleFS
String IRCommandData::toStringForFS() const {
    String data = "";
    data += String(timestamp);
    data += ",";
    data += getProtocolString();
    data += ",";
    data += "0x" + String(value, HEX); // Store as hex string
    data += ",";
    data += String(bits);
    data += ",";
    data += String(repeat ? "true" : "false");
    data += ",";

 // Append rawData as space-separated values
    data += String(rawDataLen); // First, store the length of the raw data
    for (uint16_t i = 0; i < rawDataLen; ++i) {
        data += " "; // Space as delimiter between raw numbers
        data += String(rawData[i]);
    }
    return data;

    return data;
}

// Helper to parse delimited string from FS back to struct
// This is a basic parser. For more robust parsing, consider a JSON library.
IRCommandData IRCommandData::fromStringForFS(const String& dataString) {
    IRCommandData cmd;
    cmd.timestamp = 0; // Default values
    cmd.decode_type = UNKNOWN;
    cmd.value = 0;
    cmd.bits = 0;
    cmd.address = 0;
    cmd.command = 0;
    cmd.repeat = false;
    cmd.rawDataLen = 0;

    // Parse the string: timestamp,protocol,hex_value,bits,address,command,repeat_flag
    int lastPos = 0;
    int commaPos;
    
    // 1. timestamp
    commaPos = dataString.indexOf(',', lastPos);
    if (commaPos == -1) return cmd; // Error
    cmd.timestamp = strtoull(dataString.substring(lastPos, commaPos).c_str(), NULL, 10);
    lastPos = commaPos + 1;

 // 2. protocol
    commaPos = dataString.indexOf(',', lastPos);
    if (commaPos == -1) return cmd; // Error
    String protocolStr = dataString.substring(lastPos, commaPos);
    // Convert protocol string back to enum
    if (protocolStr == "NEC") cmd.decode_type = NEC;
    else if (protocolStr == "SONY") cmd.decode_type = SONY;
    else if (protocolStr == "RC5") cmd.decode_type = RC5;
    else if (protocolStr == "RC6") cmd.decode_type = RC6;
    else if (protocolStr == "PANASONIC") cmd.decode_type = PANASONIC;
    else if (protocolStr == "JVC") cmd.decode_type = JVC;
    else if (protocolStr == "SAMSUNG") cmd.decode_type = SAMSUNG;
    else if (protocolStr == "DENON") cmd.decode_type = DENON;
    else if (protocolStr == "PRONTO") cmd.decode_type = PRONTO;
    else if (protocolStr == "RAW") cmd.decode_type = RAW;
    else cmd.decode_type = UNKNOWN;
    lastPos = commaPos + 1;

    // 3. hex_value
    commaPos = dataString.indexOf(',', lastPos);
    if (commaPos == -1) return cmd; // Error
    String hexValStr = dataString.substring(lastPos, commaPos);
    cmd.value = strtoull(hexValStr.c_str(), NULL, 16);
    lastPos = commaPos + 1;

    // 4. bits
    commaPos = dataString.indexOf(',', lastPos);
    if (commaPos == -1) return cmd; // Error
    cmd.bits = dataString.substring(lastPos, commaPos).toInt(); // toInt() is fine for uint16_t
    lastPos = commaPos + 1;

    // 5. address
    commaPos = dataString.indexOf(',', lastPos);
    if (commaPos == -1) return cmd; // Error
    cmd.address = strtoul(dataString.substring(lastPos, commaPos).c_str(), NULL, 10); // Using strtoul for uint32_t
    lastPos = commaPos + 1;

    // 6. command
    commaPos = dataString.indexOf(',', lastPos);
    if (commaPos == -1) return cmd; // Error
    cmd.command = strtoul(dataString.substring(lastPos, commaPos).c_str(), NULL, 10); // Using strtoul for uint32_t
    lastPos = commaPos + 1;

    // 7. repeat_flag (last field)
    String repeatStr = dataString.substring(lastPos);
    repeatStr.trim(); // Important to remove newline character if present
    cmd.repeat = (repeatStr == "true");

    // 8. rawDataLen and rawData (remaining part of the string)
    String rawDataSection = dataString.substring(lastPos);
    rawDataSection.trim();

    if (rawDataSection.isEmpty()) {
        cmd.rawDataLen = 0;
    } else {
        int firstSpace = rawDataSection.indexOf(' ');
        if (firstSpace == -1) {
            cmd.rawDataLen = rawDataSection.toInt();
            if (cmd.rawDataLen > 0) {
                Serial.println("Warning: Raw data length specified, but no raw values found.");
                cmd.rawDataLen = 0;
            }
        } else {
            cmd.rawDataLen = rawDataSection.substring(0, firstSpace).toInt();
            String rawValuesStr = rawDataSection.substring(firstSpace + 1);

            int currentValStart = 0;
            int nextSpace;
            uint16_t count = 0;

            while (count < cmd.rawDataLen && currentValStart < rawValuesStr.length()) {
                nextSpace = rawValuesStr.indexOf(' ', currentValStart);
                String valStr;
                if (nextSpace == -1) {
                    valStr = rawValuesStr.substring(currentValStart);
                } else {
                    valStr = rawValuesStr.substring(currentValStart, nextSpace);
                }
                valStr.trim();

                if (!valStr.isEmpty()) {
                    cmd.rawData[count++] = valStr.toInt();
                }

                if (nextSpace == -1) {
                    break;
                }
                currentValStart = nextSpace + 1;
            }
            cmd.rawDataLen = count;
        }
    }
    cmd.rawDataLen = min(cmd.rawDataLen, MAX_RAW_LEN);
    return cmd;
}

bool readIRCommand(IRCommandData &commandData) {
    if (irrecv.decode(&results)) {
    // Populate the struct from decode_results
        commandData.timestamp = millis(); // Or use NTP time if available
        commandData.decode_type = results.decode_type;
        commandData.value = results.value;
        commandData.bits = results.bits;
        commandData.address = results.address;
        commandData.command = results.command;
        commandData.repeat = results.repeat;

        // Copy raw data if available and fits
        if (results.rawlen > 0 && results.rawbuf != NULL) {
            commandData.rawDataLen = min(results.rawlen, MAX_RAW_LEN);
            Serial.print("DEBUG: Copying ");
            Serial.print(commandData.rawDataLen);
            Serial.println(" raw data entries.");
            for (uint16_t i = 0; i < commandData.rawDataLen; ++i) {
                commandData.rawData[i] = results.rawbuf[i];
            }
        } else {
            commandData.rawDataLen = 0;
            Serial.println("DEBUG: results.rawlen was 0 or results.rawbuf was NULL. rawDataLen set to 0.");
        }

        Serial.print("IR Code Received: Protocol=");
        Serial.print(commandData.getProtocolString());
        Serial.print(", Value=");
        Serial.print("0x");
        Serial.print(commandData.value, HEX);
        Serial.print(", Bits=");
        Serial.print(commandData.bits);
        Serial.print(", Repeat=");
        Serial.println(commandData.repeat ? "true" : "false");
        Serial.print(", RawLen=");
        Serial.println(commandData.rawDataLen);

        irrecv.resume();
        return true;
    }
    return false;
}

void sendIRCommand(const IRCommandData &commandData, int repeats) {
Serial.print("Attempting to send IR code: Protocol=");
    Serial.print(commandData.getProtocolString());
    Serial.print(", Value=");
    Serial.print("0x");
    Serial.print(commandData.value, HEX);
    Serial.print(", Bits=");
    Serial.print(commandData.bits);
    Serial.println(".");
    Serial.print(", RawLen=");
    Serial.print(commandData.rawDataLen);
    Serial.println(".");

    const uint16_t kDefaultIrFrequency = 38; // kHz

    // Use a switch statement to call the correct send function based on protocol
    switch (commandData.decode_type) { // Use decode_type here
        case NEC:
            irsend.sendNEC(commandData.value, commandData.bits);
            break;
        case SONY:
            irsend.sendSony(commandData.value, commandData.bits);
            break;
        case RC5:
            irsend.sendRC5(commandData.value, commandData.bits);
            break;
        case RC6:
            irsend.sendRC6(commandData.value, commandData.bits);
            break;
        case PANASONIC:
            // Panasonic requires address and command separated
            irsend.sendPanasonic(commandData.address, commandData.command);
            break;
        case JVC:
            irsend.sendJVC(commandData.value, commandData.bits, commandData.repeat);
            break;
        case SAMSUNG:
            irsend.sendSamsung36(commandData.value, commandData.bits, commandData.repeat);
            break;
        case DENON:
             irsend.sendDenon(commandData.value, commandData.bits, commandData.repeat);
             break;
        default:
            if (commandData.rawDataLen > 0) {
                // The `rawData` stores durations in half-microseconds.
                // irsend.sendRaw() expects microseconds.
                // The library's `decode` function populates `rawbuf` with ticks (half-microseconds).
                // So, we need to convert them back to microseconds for `sendRaw` if the
                // `IRutils.cpp` `resultToRawArray` function isn't used (which does this conversion).
                // However, `results.rawbuf` *already contains ticks*, so `sendRaw` expects ticks too.
                // Let's verify this, as this is a common point of confusion.
                // According to IRremoteESP8266 documentation, `sendRaw` expects ticks (half-microseconds)
                // in the array if you directly pass `results.rawbuf`.
                irsend.sendRaw(commandData.rawData, commandData.rawDataLen, kDefaultIrFrequency);
                Serial.println("Sent as RAW data.");
            } else {
                Serial.println("Warning: Cannot send RAW/UNKNOWN/PRONTO; raw data not available or empty.");
            }
            break;
    }
}