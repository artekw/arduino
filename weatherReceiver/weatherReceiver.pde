#include <RF12.h>
#include <Ports.h>
#include <weather_data.h>

#define JSON 1 //JSON or SERIAL

char str[250];
static byte ACT_LED = 9;

void setup () {
    Serial.begin(115200);
//    pinMode(ACT_LED, OUTPUT);
//    digitalWrite(ACT_LED, HIGH);
    Serial.println("\n[WeatherStation]");
    rf12_initialize(10, RF12_433MHZ, 212);
//    rf12_control(0xC647);
}

void loop () {
    if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0 && rf12_len == sizeof pomiar) {
        memcpy(&pomiar, (void*) rf12_data, sizeof pomiar);
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
          transmissionRF();
//          activityLed(1);
//          activityLed(0);
        }
        else {
          Serial.print("WS ");
//          Serial.print("LP");
//          Serial.print(pomiar.seq);
          Serial.print(" L");
          Serial.print(pomiar.light, DEC);
          Serial.print(" H");
          Serial.print(pomiar.humi);
          Serial.print(" T");
          Serial.print(pomiar.temp);
          Serial.print(" P");
          Serial.print(pomiar.pressure);
//          Serial.print(" S");
//          Serial.print(pomiar.wind);
          Serial.print(" W" );
          Serial.print(pomiar.lobat, DEC);
          Serial.print(" BV");
          Serial.print(pomiar.battvol);
          Serial.print(" SV");
          Serial.print(pomiar.solvol);
          Serial.print(" SOL");
          Serial.print(pomiar.solar, DEC);
          Serial.print(" BAT");
          Serial.print(pomiar.bat, DEC);
          Serial.println();
       }
       memset(str, 0, sizeof(str)); // clear
   }
   
}

void transmissionRF()
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
//  addJSON(str,"SP", pomiar.wind);
  addJSON(str,"BV",  pomiar.battvol);
  addJSON(str,"SV",  pomiar.solvol);
  addJSON(str,"LB", pomiar.lobat);
  addJSON(str,"SOL", pomiar.solar);
  addJSON(str,"BAT", pomiar.bat);
  endJSON(str);
}

static void activityLed (byte on) {
  digitalWrite(ACT_LED, !on);
  delay(150);
}
