#include "pins.h"
#include "ControleIrUtils.h"

int DELAY_BETWEEN_REPEAT = 50;

IRrecv irrecv(IR_RECV_PIN);
IRsend irsend(IR_SEND_PIN);
decode_results results;

void irInit()
{
    irrecv.enableIRIn();
    irsend.begin();
}

bool readIRCommand(String &commandCode) {
    if (irrecv.decode(&results)) {
        // We have received an IR signal
        // Print the results to the serial monitor for debugging
        Serial.print("IR Code: ");
        String tempCommand = resultToHexidecimal(&results);
        Serial.println(tempCommand);

        // You might want to filter or process the command further here
        // For now, just store the hex representation
        commandCode = tempCommand;

        irrecv.resume(); // Resume receiving
        return true;
    }
    return false;
}

// void sendIRCommand(const String &hexCode, int repeats) {

// }