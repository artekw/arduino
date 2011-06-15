#include <RF12.h>
#include <Ports.h>
#include <weather_data.h>

#define JSON 0
#define ACT_LED 3
#define NODEID 5

char str[200];

void setup () {
    Serial.begin(115200);
    Serial.println("\n[WeatherStation]");
    rf12_initialize(NODEID, RF12_433MHZ, 212);
}

void loop () {
    if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0 && rf12_len == sizeof pomiar) {
        memcpy(&pomiar, (void*) rf12_data, sizeof pomiar);
        // doszlo ?
        if (RF12_WANTS_ACK) {
          if (!JSON) {
            Serial.println("ack");
          }
          rf12_sendStart(RF12_ACK_REPLY, 0, 0);
        }
        rf12_recvDone();
        if (JSON) {
          createJSON();
          transmissionRS();
        }
        else {
          Serial.print("WS ");
          Serial.print("LP");
          Serial.print(pomiar.seq);
          Serial.print(" L");
          Serial.print(pomiar.light, DEC);
          Serial.print(" H");
          Serial.print(pomiar.humi);
          Serial.print(" T");
          Serial.print(pomiar.temp);
          Serial.print(" P");
          Serial.print(pomiar.pressure);
          Serial.print(" S");
          Serial.print(pomiar.wind);
          Serial.print(" W" );
          Serial.print(pomiar.lobat, DEC);
          Serial.print(" B");
          Serial.print(pomiar.battvol);
          Serial.print(" SOL");
          Serial.print(pomiar.solar, DEC);
          Serial.println();
       }
       memset(str, 0, sizeof(str)); // czyscimy tablice
   }
   
}

void transmissionRS()
{
  Serial.println(str);
}

void createJSON()
{  
  srtJSON(str);
  addJSON(str,"LDR",  pomiar.light);
  addJSON(str,"RH",  pomiar.humi);
  addJSON(str,"T",  pomiar.temp);
  addJSON(str,"PR",  pomiar.pressure);
  addJSON(str,"SP", pomiar.wind);
  addJSON(str,"VOL",  pomiar.battvol);
  addJSON(str,"LB", pomiar.lobat);
  addJSON(str,"SOL", pomiar.solar);
  endJSON(str);
}

static void activityLed (byte on) {
  pinMode(ACT_LED, OUTPUT);
  digitalWrite(ACT_LED, on);
  delay(150);
}
