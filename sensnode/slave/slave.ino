#include <JeeLib.h>

//MilliTimer sendTimer;

#define thisNode 1

typedef struct {
    byte destnode;
    byte cmd;
    byte state :1;
} Payload;
Payload rxdata;

void setup () {
    Serial.begin(9600);
    Serial.println("Receive");
    rf12_initialize(thisNode, RF12_433MHZ, 210);
    pinMode(9, OUTPUT);
}

void loop () {
    /* 12 02 1  |  NID CMD STATE */
    if (rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof rxdata) {

        Serial.print("RX");
        Serial.print(' ');
        Serial.print((int) rf12_hdr);
        for (byte i = 0; i < rf12_len; ++i) {
            Serial.print(' ');
            Serial.print((int) rf12_data[i]);
        }
        Serial.println();

        memcpy(&rxdata, (void*) rf12_data, sizeof rxdata);

    }

    if (rxdata.destnode == thisNode) {

        /* process cmd */

        switch (rxdata.state) {
            case 0:
                digitalWrite(9, LOW);
                break;
            case 1:
                digitalWrite(9, HIGH);
                break;
            default:
                digitalWrite(9, LOW);
        }
    }
/*
    if (sendTimer.poll(3000))
        needToSend = 1;

    if (needToSend && rf12_canSend()) {
        needToSend = 0;

        sendLed(1);
        rf12_sendStart(0, payload, sizeof payload);
        delay(100); // otherwise led blinking isn't visible
        sendLed(0);
    }*/
}