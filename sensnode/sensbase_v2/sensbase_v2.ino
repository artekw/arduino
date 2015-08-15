// sensbase v2.0

#define RF69_COMPAT 0

#include <JeeLib.h>

/*************************************************************/
// Output
#define BINARY 1 // rf12demo https://github.com/jcw/jeelib/blob/master/examples/RF12/RF12demo/RF12demo.ino
#define DEBUG 0

static byte ACT_LED       = 9;
static byte NODEID        = 30;  // 31 for any node(all)
static byte BAND          = 210; // 0-255
static int BAUD           = 9600; // serial speed

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
  Serial.begin(BAUD);
  rf12_initialize(NODEID, RF12_433MHZ, BAND);
  pinMode(ACT_LED, OUTPUT);
  activityLed(0);
}

void loop () {
  activityLed(0);
  if (rf12_recvDone() & rf12_crc == 0) {
    //int node_id = (rf12_hdr & 0x1F);
      if (BINARY) {
        Serial.print("OK");
        Serial.print(' ');
        Serial.print((int) rf12_hdr);
        for (byte i = 0; i < rf12_len; ++i) {
          Serial.print(' ');
          Serial.print((int) rf12_data[i]);
        }
        Serial.print(' ');
        Serial.println();
        activityLed(1);
      }
    if (RF12_WANTS_ACK) {
        rf12_sendNow(RF12_ACK_REPLY,0,0);
    }
    //memset(str, 0, sizeof(str)); // clear
  }
  // sending part

  if (Serial.available() >= 5) {
    /* RX 12 02 1  |  NID CMD STATE */
    needToSend = 1;

    int rxb = Serial.available();
    for (int i = 0; i<rxb; i++) {
      ramka[i] = (Serial.read()-48);
    }

    if (DEBUG) {
      Serial.println("Wyslano");
    }

    rxdata.destnode = (ramka[0] * 10) + ramka[1];
    rxdata.cmd = (ramka[2] * 10) + ramka[3];
    rxdata.state = ramka[4];
  }
  if (needToSend && rf12_canSend()) {
    needToSend = 0;
    rf12_sendStart(0, &rxdata, sizeof rxdata);
  }

}

static void activityLed (byte on) {
  digitalWrite(ACT_LED, !on);
  delay(150);
}
