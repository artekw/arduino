#include <GLCD_ST7565.h>		
#include <JeeLib.h>
#include <weather_data.h>
#include <avr/pgmspace.h>

#include "utility/font_clR6x8.h"
#include "utility/font_clR4x6.h"
#include "utility/font_clR6x6.h"
#include "utility/font_courB18.h"
#include "utility/font_ncenBI14.h"
#include "utility/font_ncenR08.h""

#define JSONRS 1 //JSON or SERIAL
#define ACK    1

char str[250];
static byte ACT_LED         = 9; 
static byte NODEID          = 31; // 31 - any node
static byte NODEGROUP       = 212;

unsigned long last;

GLCD_ST7565 glcd;

extern byte gLCDBuf[1024];

void setup () {
  #if (JSONRS)
    Serial.begin(115200);
  #endif
  rf12_initialize(NODEID, RF12_433MHZ, NODEGROUP);
  glcd.begin();
  glcd.backLight(255);
  last = millis();
}

void loop () {
    if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0 && rf12_len == sizeof measure) {
        memcpy(&measure, (void*) rf12_data, sizeof measure);
        last = millis();
        #if (ACK)
          if (RF12_WANTS_ACK) {
            rf12_sendStart(RF12_ACK_REPLY, 0, 0);
          }
        #endif  
        if (JSONRS) {
          createJSON();
          outRS();
        }
        GLCD();
    }
    memset(str, 0, sizeof(str)); // clear
    /*
    if (millis()>lastTemp+10000){
      sensors.requestTemperatures();
      temp=(sensors.getTempCByIndex(0));
      lastTemp=millis();
      
   }*/
}

void outRS()
{
  Serial.println(str);
}

static void refreshAndWait() {	
    glcd.refresh();
    glcd.clear();
    delay(1000);		
}

void GLCD() {
  activityLed(1);
  glcd.setFont(font_clR6x6);
  char stra[50];
  itoa(measure.nodeid, stra, 10);  
  glcd.drawString(0, 0, "ID Punktu: ");
  glcd.drawString(85, 0, stra);
  
  // press
  itoa(measure.pressure, stra, 10);
  strcat(stra,"hPa");
  glcd.drawString(0, 8, "Cisnienie:");
  glcd.drawString(85, 8, stra);

  // temp
  itoa(measure.temp, stra, 10);
  strcat(stra,"C");
  glcd.drawString(0, 15, "Temperatura:");
  glcd.drawString(85, 15, stra);
  
  // humi
  itoa(measure.humi, stra, 10);
  strcat(stra,"%");
  glcd.drawString(0, 23, "Wilgotnosc:");
  glcd.drawString(85, 23, stra);
  
  // light
  itoa(measure.light, stra, 10);
  glcd.drawString(0, 30, "Jasnosc:");
  glcd.drawString(85, 30, stra);
 
  // batvol
  itoa(measure.battvol, stra, 10);
  glcd.drawString(0, 37, "Nap. zas:");
  glcd.drawString(85, 37, stra);
  
  // last upd
//  glcd.setFont(font_clR4x6);
//  glcd.drawString(0,57, "Last update: ");
//  int seconds = (int)((millis()-last)/1000.0);
//  itoa(seconds,stra,10);
//  strcat(stra,"s ago");
//  glcd.drawString(50,57,stra);

  glcd.drawString(0, 50, "Poz.napiecia:");
  glcd.drawRect(0, 57, 127, 7, WHITE);
  glcd.fillRect(0, 57, (measure.battvol)*0.252, 7, WHITE);
  
  //update
  refreshAndWait();
  activityLed(0);
}
void createJSON()
{  
  srtJSON(str);
  addJSON(str,"nodeid", measure.nodeid);
  addJSON(str,"light", measure.light);
  addJSON(str,"humi", measure.humi);
  addJSON(str,"temp", measure.temp);
  addJSON(str,"press", measure.pressure);
  addJSON(str,"batvol", measure.battvol);
  addJSON(str,"solvol", measure.solvol);
  addJSON(str,"lobat", measure.lobat);
  addJSON(str,"solon", measure.solar);
  addJSON(str,"baton", measure.bat);
  endJSON(str);
}

static void activityLed (byte on) {
  pinMode(ACT_LED, OUTPUT);
  digitalWrite(ACT_LED, !on);
  delay(150);
}
