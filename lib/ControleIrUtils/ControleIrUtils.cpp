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
        case RAW: return "RAW"; // This means it was only decoded as raw
        case UNKNOWN: return "UNKNOWN";
        // Add more cases for other protocols you care about
        default: return "UNKNOWN_PROTOCOL";
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

        Serial.print("IR Code Received: Protocol=");
        Serial.print(commandData.getProtocolString());
        Serial.print(", Value=");
        Serial.print("0x");
        Serial.print(commandData.value, HEX);
        Serial.print(", Bits=");
        Serial.print(commandData.bits);
        Serial.print(", Repeat=");
        Serial.println(commandData.repeat ? "true" : "false");

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
        case PRONTO:
             // Pronto codes are typically raw, requiring a different approach.
             // You'd need to store the rawData array and its length for this.
             Serial.println("Warning: PRONTO protocol detected. Requires raw data for re-sending.");
             break;
        case RAW:
            // If you stored raw data (rawbuf, rawlen), you'd use irsend.sendRaw() here.
            Serial.println("Warning: RAW protocol detected. Cannot re-send without raw buffer data.");
            break;
        case UNKNOWN:
            Serial.println("Warning: UNKNOWN protocol. Cannot re-send.");
            break;
        default:
            Serial.println("Error: Unhandled protocol for sending.");
            break;
    }
}