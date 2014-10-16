#include <JeeLib.h>
#include <PCF8574.h>
#include <Wire.h>

//MilliTimer sendTimer;
PCF8574 i2crelay_1;

#define thisNode 1
#define relay_adress 0x3F // 111(HHH)

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

    i2crelay_1.begin(relay_adress);
    i2crelay_1.pinMode(0, OUTPUT); //rel1
    i2crelay_1.pinMode(1, OUTPUT); //rel2
    i2crelay_1.pinMode(2, OUTPUT); //rel3
    i2crelay_1.pullUp(0); // ??
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
                i2crelay_1.digitalWrite(0, HIGH);
                break;
            case 1:
                i2crelay_1.digitalWrite(0, LOW);
                break;
            default:
                i2crelay_1.digitalWrite(0, LOW);
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