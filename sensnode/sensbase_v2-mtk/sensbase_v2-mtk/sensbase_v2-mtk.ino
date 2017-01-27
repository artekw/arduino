#include <Arduino.h>

// sensbase v2.0-linkit
// Version for LinkIt Smart7688 Duo

#define RF69_COMPAT 1     // RFM69

// #include <Bridge.h>
#include <JeeLib.h>

/*************************************************************/
#define BINARY        1    // binary output
#define DEBUG         0    // for devs
#define ACT_LED_TX    8    // D8 / red
#define ACT_LED_RX    7    // D7 / green
#define NODEID        31   // 31 for any node(all)
#define NETWORK       210  // 0-255
#define SENSMON_BAUD  9600 // in OpenWrt /dev/ttyS0
/*************************************************************/


char ramka[5];
byte needToSend;

// structure of data

typedef struct {
    byte destnode;
    byte cmd;
    byte state :1;
} Payload;
Payload rxdata;


void setup () {
  // Bridge.begin();        // initialize Bridge
  Serial1.begin(SENSMON_BAUD);
  rf12_initialize(NODEID, RF12_433MHZ, NETWORK);
  pinMode(ACT_LED_RX, OUTPUT);
  pinMode(ACT_LED_TX, OUTPUT);
  activityLed(0,0); //rx
  activityLed(0,1); //tx
}


void loop () {
  activityLed(0, 0); //rx
  if (rf12_recvDone() & rf12_crc == 0) {
    //int node_id = (rf12_hdr & 0x1F);
      if (BINARY) {
        Serial1.print("OK");
        Serial1.print(' ');
        Serial1.print((int) rf12_hdr);
        for (byte i = 0; i < rf12_len; ++i) {
          Serial1.print(' ');
          Serial1.print((int) rf12_data[i]);
        }
        Serial1.print(' ');
        Serial1.println();
        activityLed(1, 0); //rx
      }
    if (RF12_WANTS_ACK) {
        rf12_sendNow(RF12_ACK_REPLY,0,0);
    }
    //memset(str, 0, sizeof(str)); // clear
  }
  // sending part

  if (Serial1.available() >= 5) {
    /* RX 12021  |  NID CMD STATE */
    needToSend = 1;
    int rxb = Serial1.available();
    for (int i = 0; i<rxb; i++) {
      ramka[i] = (Serial1.read()-48);
    }

    if (DEBUG) {
      Serial1.println("Wyslano");
    }

    rxdata.destnode = (ramka[0] * 10) + ramka[1];
    rxdata.cmd = (ramka[2] * 10) + ramka[3];
    rxdata.state = ramka[4];
  }
  if (needToSend && rf12_canSend()) {
    needToSend = 0;
    activityLed(1, 1); //tx
    rf12_sendStart(0, &rxdata, sizeof rxdata);
    activityLed(0, 1); //tx
  }

}


static void activityLed (byte on, byte dir) {
  // rx
  if (dir == 0 ) {
    digitalWrite(ACT_LED_RX, on);
    delay(150);
  }
  // tx
  else {
    digitalWrite(ACT_LED_TX, on);
    delay(150);
  }
}
