// sensbase v2.0

#include <JeeLib.h>

// NodeIDs: 0-31
// weather nodes nodeids: 0-20
// power nodes nodeids: 21-30

#define JSON 1 // JSON
#define SERIAL 0 // Serial

char str[250];
static byte ACT_LED       = 9;
static byte NODEID        = 10;  // 31 for any node(all)
static byte NODEGROUP     = 210; // 0-255


typedef struct {
  byte nodeid;
  int value;
  int battvol;
} Payload;
Payload testdata;

void setup () {
  Serial.begin(9600);
  Serial.println("Test");
  rf12_initialize(NODEID, RF12_433MHZ, NODEGROUP);
  pinMode(ACT_LED, OUTPUT);
  activityLed(1);
}

void loop () {
    if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0 && rf12_len == sizeof testdata) {
        memcpy(&testdata, (void*) rf12_data, sizeof testdata);
        activityLed(1);
        if (RF12_WANTS_ACK) {
          #if (!JSON)
            activityLed(1);
            Serial.println("-----");
            Serial.println("-> ack");
            activityLed(0);
          #endif
          rf12_sendStart(RF12_ACK_REPLY, 0, 0);
        }
        if (JSON) {        
          createJSON();
          transmission();
        }
        if (SERIAL) {
          Serial.flush();
          Serial.print(testdata.nodeid);
          Serial.print(" ");
          Serial.print(testdata.value);
          Serial.print(" ");
          Serial.print(testdata.battvol);
          Serial.println(" ");
        }
       memset(str, 0, sizeof(str)); // clear
   }
}

void transmission()
{
  Serial.println(str);
}

void createJSON()
{  
  srtJSON(str);
  addJSON(str,"nodeid", testdata.nodeid);
  addJSON(str,"value",  testdata.value);
  addJSON(str,"batvol",  testdata.battvol);
  endJSON(str);
}

static void activityLed (byte on) {
  digitalWrite(ACT_LED, !on);
  delay(150);
}
