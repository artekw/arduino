#include <JeeLib.h>
#include <avr/pgmspace.h>

#define JSON 1 // JSON
#define SERIAL 0 // Serial

char str[250];
static byte ACT_LED       = 9;
static byte NODEID        = 31;  // 31 for any node(all)
static byte NODEGROUP     = 212;

// structure of data
typedef struct {
	int nodeid;
	int light;
	float humi;
	float temp;
	float pressure;
	byte lobat		:1;
	int battvol;
	int solvol;
	byte solar		:1;
	byte bat		:1;
} Payload;
Payload measure;

void setup () {
  Serial.begin(115200);
  rf12_initialize(NODEID, RF12_433MHZ, NODEGROUP);
  pinMode(ACT_LED, OUTPUT);
  activityLed(0);
}

void loop () {
    if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0 && rf12_len == sizeof measure) {
        memcpy(&measure, (void*) rf12_data, sizeof measure);
        activityLed(1);
        delay(2);
        activityLed(0);
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
          Serial.print(measure.temp);
          Serial.print(" ");
          Serial.print(measure.pressure);
          Serial.print(" ");
          Serial.print(measure.humi);
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
  addJSON(str,"nodeid", measure.nodeid);
  addJSON(str,"light",  measure.light);
  addJSON(str,"humi",  measure.humi);
  addJSON(str,"temp",  measure.temp);
  addJSON(str,"press",  measure.pressure);
  addJSON(str,"batvol",  measure.battvol);
  addJSON(str,"solvol",  measure.solvol);
  addJSON(str,"lobat", measure.lobat);
  addJSON(str,"solon", measure.solar);
  addJSON(str,"baton", measure.bat);
  endJSON(str);
}

static void activityLed (byte on) {
  digitalWrite(ACT_LED, !on);
  delay(150);
}
