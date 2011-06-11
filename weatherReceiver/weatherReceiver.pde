#include <RF12.h>
#include <Ports.h>
#include <weather_data.h>

#define JSON 1

char str[200];

void setup () {
    Serial.begin(115200);
    Serial.println("\n[WeatherStation]");
    rf12_initialize('R', RF12_433MHZ, 212);
}

void loop () {
    if (rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof pomiar) {
        memcpy(&pomiar, (byte*) rf12_data, sizeof pomiar);
        if (JSON) {
          createJSON();
          transmissionRS();
        }
        else {
          Serial.print("WS ");
          Serial.print("L");
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
  endJSON(str);
}
