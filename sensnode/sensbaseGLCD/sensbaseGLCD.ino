#include <GLCD_ST7565.h>		
#include <JeeLib.h>
#include <avr/pgmspace.h>

#include "utility/font_helvB24.h"
#include "utility/font_helvB14.h"
#include "utility/font_helvB12.h"
#include "utility/font_clR4x6.h"
#include "utility/font_clR6x8.h"

char str[250];
static byte NODEID          = 20; 
static byte NODEGROUP       = 212;

static byte brightness      = 9;
static byte red_led         = 14;

unsigned long last;

GLCD_ST7565 glcd;

// structure of data
typedef struct {
	int nodeid;
	int light;
	float humi;
	float temp;
	float pressure;
	byte lobat		:1;
	int battvol;
	byte fet		:1;
} Payload;
Payload measure;


void setup () {
  Serial.begin(115200);
  rf12_initialize(NODEID, RF12_433MHZ, NODEGROUP);
  glcd.begin();
  glcd.backLight(200);
  last = millis();
  glcd.clear();
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
}

void loop () {
  if (rf12_recvDone())
  { 
    if (rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0) {
      measure = *(Payload*) rf12_data; 
      last = millis();
    }
//  if (measure.nodeid == 2) {
    glcd.clear();
    draw_node_page();
    glcd.refresh();
//  }
}
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
 // activityLed(1);
  glcd.setFont(font_clR6x8);
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
  glcd.setFont(font_clR4x6);
  glcd.drawString(0,57, "Last update: ");
  int seconds = (int)((millis()-last)/1000.0);
  itoa(seconds,stra,10);
  strcat(stra,"s ago");
  glcd.drawString(50,57,stra);

//  glcd.drawString(0, 50, "Poz.napiecia:");
//  glcd.drawRect(0, 57, 127, 7, WHITE);
//  glcd.fillRect(0, 57, (measure.battvol)*0.252, 7, WHITE);
  
  //update
  refreshAndWait();

  //activityLed(0);
}
/*
static void activityLed (byte on) {
  pinMode(ACT_LED, OUTPUT);
  digitalWrite(ACT_LED, !on);
  delay(150);
}
*/
void draw_node_page()
{
  char stra[50];
//  glcd.fillRect(0,0,128,64,0);
  glcd.setFont(font_helvB14);
  itoa(measure.temp, stra, 10);
  glcd.drawString(0, 0, "Temp:");
  strcat(stra, "C");
  glcd.drawString(60, 0, stra);
}
