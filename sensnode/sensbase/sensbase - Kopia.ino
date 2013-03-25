// sensbase v2.0

#include <JeeLib.h>

// NodeIDs: 0-31
// weather nodes nodeids: 0-20
// power nodes nodeids: 21-30

// OUTPUT
#define JSON 0 // JSON
#define BINARY 1 // rf12demo https://github.com/jcw/jeelib/blob/master/examples/RF12/RF12demo/RF12demo.ino
#define SERIAL 0 // Serial

char str[250];
static byte ACT_LED       = 9;
static byte NODEID        = 31;  // 31 for any node(all)
static byte NODEGROUP     = 212; // 0-255

int sum_power = 0;

// structure of data
/*
typedef struct {
	byte nodeid;
	int light;
	float humi;
	float temp;
	float pressure;
	byte lobat		:1;
	int battvol;
} Payload;
Payload measure;
*/

typedef struct {
  byte nodeid;
  int light;
  float humi;
  float temp;
  float pressure;
  /* power */
  int power1;
  int power2;
  int power3;
  int power4;
  float Vrms;
  byte lobat    :1;
  int battvol;
} Payload;
Payload measure;

/*
typedef struct {
	byte nodeid;
} PayloadOut;
Payload request;
*/
void setup () {
  Serial.begin(9600);
  rf12_initialize(NODEID, RF12_433MHZ, NODEGROUP);
  pinMode(ACT_LED, OUTPUT);
  activityLed(0);
}

void loop () {
    if (rf12_recvDone() && (rf12_hdr & RF12_HDR_CTL) == 0 && rf12_len == sizeof measure) {
        //int node_id = (rf12_hdr & 0x1F);

        memcpy(&measure, (void*) rf12_data, sizeof measure);
        // suma mocy
        sum_power = abs(measure.power1 + measure.power2 + measure.power3 + measure.power4);
        activityLed(1);
        delay(2);
        activityLed(0);
        // crc OK
        if (rf12_crc == 0) {
          if (BINARY) {
            //todo
            byte n = rf12_len;
            Serial.print("OK");
            Serial.print(' ');
            Serial.print((int) rf12_hdr);
            for (byte i = 0; i < n; ++i) {
              Serial.print(' ');
              Serial.print((int) rf12_data[i]);
            }
            Serial.println();
          }

          if (JSON) {        
            createJSON();
            transmission();
          }
          if (RF12_WANTS_ACK) {
          #if (!JSON)
            activityLed(1);
            Serial.println("-----");
            Serial.println("-> ack");
            activityLed(0);
          #endif
          rf12_sendStart(RF12_ACK_REPLY, 0, 0);
        }
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
  addJSON(str,"power1",  measure.power1);
  addJSON(str,"power2",  measure.power2);
  addJSON(str,"power3",  measure.power3);
  addJSON(str,"power4",  measure.power4);
  addJSON(str,"sumpower", sum_power); // liczymy to tutaj
  addJSON(str,"vrms",  measure.Vrms);
  addJSON(str,"batvol",  measure.battvol);
  addJSON(str,"lobat", measure.lobat);
  endJSON(str);
}

static void activityLed (byte on) {
  digitalWrite(ACT_LED, !on);
  delay(150);
}
