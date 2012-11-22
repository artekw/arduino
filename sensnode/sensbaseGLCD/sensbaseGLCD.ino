#include <GLCD_ST7565.h>		
#include <JeeLib.h>

#include "utility/font_8x13B.h"
#include "utility/font_clR6x8.h"
#include "utility/font_clR6x6.h"
#include "utility/font_clR5x8.h"
#include "utility/font_5x7.h"
#include "utility/font_4x6.h"

// choose right utilization
#define PowerMeter 0
#define WeatherStation 1 

int send_id = 2;

int cval_use;
double vrms;
double usekwh = 0;
unsigned long last_upd;

GLCD_ST7565 glcd;

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

#if PowerMeter 
  typedef struct { int power1, power2, power3, power4; double Vrms;} Payload;
#endif
#if WeatherStation
  typedef struct { byte nodeid; int light; float humi, temp, pressure; byte lobat  :1; int battvol; } Payload;
#endif
Payload measure;

void setup () {
  #if PowerMeter
    rf12_initialize(1, RF12_433MHZ, 210);
  #endif
  #if WeatherStation
    rf12_initialize(1, RF12_433MHZ, 212);
  #endif
  glcd.begin();
  glcd.backLight(150);
  glcd.refresh();
  showInfo();
}

void loop () {
  if (rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof measure) {
    int node_id = (rf12_hdr & 0x1F);    // get node id
    if (node_id == send_id) {
      memcpy(&measure, (void*) rf12_data, sizeof measure);
      last_upd = millis();
      calculation();
      showInfo();
    }
  }
}

static void calculation() {
  #if PowerMeter 
    cval_use = cval_use + (measure.power1 - cval_use)*0.50;
    usekwh += (measure.power1 * 0.2) / 3600000;
  #endif
}

static void showInfo() {
  char buf[10];
  glcd.clear();
  glcd.setFont(font_5x7);
  #if PowerMeter 
    glcd.drawString_P(70,  0, PSTR("power W"));
    glcd.drawString_P(70,  22, PSTR("voltage V"));
    glcd.drawString_P(70,  44, PSTR("use kWh"));
  #endif
  #if WeatherStation
    glcd.drawString_P(70,  0, PSTR("press hPa"));
    glcd.drawString_P(70,  22, PSTR("temp *C"));
    glcd.drawString_P(70,  44, PSTR("humi %"));
  #endif
  // lasu update
  glcd.setFont(font_4x6);
  glcd.drawString_P(5,  55, PSTR("next upd"));
  itoa(300-((millis()-last_upd)/1000), buf, 10);
  strcat(buf,"s");
  glcd.drawString(42, 55, buf);

  glcd.setFont(font_8x13B);
  #if PowerMeter
    itoa((int)cval_use, buf, 10);
    glcd.drawString(72, 8, buf);
    dtostrf(measure.Vrms, 0, 1, buf); 
    glcd.drawString(72, 30, buf);
    dtostrf(usekwh, 0, 1, buf);
    glcd.drawString(72, 52, buf);
  #endif
  #if WeatherStation
    dtostrf(measure.pressure, 0, 1, buf);
    glcd.drawString(72, 8, buf);
    dtostrf(measure.temp, 0, 1, buf); 
    glcd.drawString(72, 30, buf);
    dtostrf(measure.humi, 0, 1, buf);
    glcd.drawString(72, 52, buf);
  #endif
  glcd.refresh();
}
  