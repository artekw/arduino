// sensbase v2.0

#include <JeeLib.h>

// Output
#define BINARY 1 // rf12demo https://github.com/jcw/jeelib/blob/master/examples/RF12/RF12demo/RF12demo.ino

char str[100];
static byte ACT_LED       = 9;
static byte NODEID        = 31;  // 31 for any node(all)
static byte NODEGROUP     = 210; // 0-255

void setup () {
  Serial.begin(9600);
  rf12_initialize(NODEID, RF12_433MHZ, NODEGROUP);
  pinMode(ACT_LED, OUTPUT);
  activityLed(0);
}

void loop () {
    if (rf12_recvDone()) {
        activityLed(1);
        delay(2);
        activityLed(0);
        // crc OK
        if (rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0) {
          int node_id = (rf12_hdr & 0x1F);
          if (BINARY) {
            byte n = rf12_len;
            Serial.print("OK");
            Serial.print(' ');
            Serial.print((int) rf12_hdr);
            for (byte i = 0; i < n; ++i) {
              Serial.print(' ');
              Serial.print((int) rf12_data[i]);
            }
            Serial.print(' ');
            Serial.println();
          }
          if (RF12_WANTS_ACK) {
            rf12_sendStart(RF12_ACK_REPLY, 0, 0);
        }
      }
       memset(str, 0, sizeof(str)); // clear
   }
}

static void activityLed (byte on) {
  digitalWrite(ACT_LED, !on);
  delay(150);
}
