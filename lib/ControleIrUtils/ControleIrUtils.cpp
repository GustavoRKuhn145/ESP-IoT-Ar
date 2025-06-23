#include "ControleIrUtils.h"
#include "pins.h"

#include <IRremote.hpp>
#include <Arduino.h>

int DELAY_BETWEEN_REPEAT = 50;


void irInit()
{
    IrReceiver.begin(IR_RECV_PIN, ENABLE_LED_FEEDBACK);
    IrSender.begin(IR_TRANS_PIN);
}

bool storeCode() {
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
        Serial.println(F("Ignore repeat"));
        return false;
    }
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_AUTO_REPEAT) {
        Serial.println(F("Ignore autorepeat"));
        return false;
    }
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_PARITY_FAILED) {
        Serial.println(F("Ignore parity error"));
        return false;
    }
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW) {
        Serial.println(("Overflow occurred, raw data did not fit into ", (RAW_BUFFER_LENGTH), " byte raw buffer"));
        return false;
    }

    if (IrReceiver.decodedIRData.rawDataPtr->rawlen < 8){
        return false;
    }

    sStoredIRData.receivedIRData = IrReceiver.decodedIRData;

    // processamento para armazenamento do código
    auto tProtocol = sStoredIRData.receivedIRData.protocol;
    if (tProtocol == UNKNOWN || tProtocol == PULSE_WIDTH || tProtocol == PULSE_DISTANCE) {
        // TODO: support PULSE_WIDTH and PULSE_DISTANCE with IrSender.write
        sStoredIRData.rawCodeLength = IrReceiver.decodedIRData.rawDataPtr->rawlen - 1;
        /*
         * Store the current raw data in a dedicated array for later usage
         */
        IrReceiver.compensateAndStoreIRResultInArray(sStoredIRData.rawCode);
        /*
         * Print info
         */
        Serial.print(F("Received unknown code and store "));
        Serial.print(IrReceiver.decodedIRData.rawDataPtr->rawlen - 1);
        Serial.println((" timing entries as raw in buffer of size ", RAW_BUFFER_LENGTH));
        IrReceiver.printIRResultRawFormatted(&Serial, true); // Output the results in RAW format

    } else {
        IrReceiver.printIRResultShort(&Serial);
        IrReceiver.printIRSendUsage(&Serial);
        sStoredIRData.receivedIRData.flags = 0; // clear flags -esp. repeat- for later sending
        Serial.println();
    }

    return true;
}

void sendCode(storedIRDataStruct *aIRDataToSend) {
    auto tProtocol = aIRDataToSend->receivedIRData.protocol;
    if (tProtocol == UNKNOWN || tProtocol == PULSE_WIDTH || tProtocol == PULSE_DISTANCE /* i.e. raw */) {
        // Assume 38 KHz
        IrSender.sendRaw(aIRDataToSend->rawCode, aIRDataToSend->rawCodeLength, 38);

        Serial.print(F("raw "));
        Serial.print(aIRDataToSend->rawCodeLength);
        Serial.println(F(" marks or spaces"));
    } else {
        /*
         * Use the write function, which does the switch for different protocols
         */
        IrSender.write(&aIRDataToSend->receivedIRData);
        printIRResultShort(&Serial, &aIRDataToSend->receivedIRData, true);
    }
}

// void storeCode()
// {
//     if (IrReceiver.decodedIRData.rawDataPtr->rawlen < 4) {
//         Serial.print(F("Ignore data with rawlen="));
//         Serial.println(IrReceiver.decodedIRData.rawDataPtr->rawlen);
//         return;
//     }
//     if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
//         Serial.println(F("Ignore repeat"));
//         return;
//     }
//     if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_AUTO_REPEAT) {
//         Serial.println(F("Ignore autorepeat"));
//         return;
//     }
//     if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_PARITY_FAILED) {
//         Serial.println(F("Ignore parity error"));
//         return;
//     }
//     if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW) {
//         Serial.println(("Overflow occurred, raw data did not fit into ", (RAW_BUFFER_LENGTH), " byte raw buffer"));
//         return;
//     }
//     /*
//      * Copy decoded data
//      */
//     sStoredIRData.receivedIRData = IrReceiver.decodedIRData;

//     auto tProtocol = sStoredIRData.receivedIRData.protocol;
//     if (tProtocol == UNKNOWN || tProtocol == PULSE_WIDTH || tProtocol == PULSE_DISTANCE) {
//         // TODO: support PULSE_WIDTH and PULSE_DISTANCE with IrSender.write
//         sStoredIRData.rawCodeLength = IrReceiver.decodedIRData.rawDataPtr->rawlen - 1;
//         /*
//          * Store the current raw data in a dedicated array for later usage
//          */
//         IrReceiver.compensateAndStoreIRResultInArray(sStoredIRData.rawCode);
//         /*
//          * Print info
//          */
//         Serial.print(F("Received unknown code and store "));
//         Serial.print(IrReceiver.decodedIRData.rawDataPtr->rawlen - 1);
//         Serial.println((" timing entries as raw in buffer of size ", RAW_BUFFER_LENGTH));
//         IrReceiver.printIRResultRawFormatted(&Serial, true); // Output the results in RAW format

//     } else {
//         IrReceiver.printIRResultShort(&Serial);
//         IrReceiver.printIRSendUsage(&Serial);
//         sStoredIRData.receivedIRData.flags = 0; // clear flags -esp. repeat- for later sending
//         Serial.println();
//     }
// }

// void sendCode(storedIRDataStruct *aIRDataToSend) {
//     auto tProtocol = aIRDataToSend->receivedIRData.protocol;
//     if (tProtocol == UNKNOWN || tProtocol == PULSE_WIDTH || tProtocol == PULSE_DISTANCE /* i.e. raw */) {
//         // Assume 38 KHz
//         IrSender.sendRaw(aIRDataToSend->rawCode, aIRDataToSend->rawCodeLength, 38);

//         Serial.print(F("raw "));
//         Serial.print(aIRDataToSend->rawCodeLength);
//         Serial.println(F(" marks or spaces"));
//     } else {
//         /*
//          * Use the write function, which does the switch for different protocols
//          */
//         IrSender.write(&aIRDataToSend->receivedIRData);
//         printIRResultShort(&Serial, &aIRDataToSend->receivedIRData, true);
//     }
// }