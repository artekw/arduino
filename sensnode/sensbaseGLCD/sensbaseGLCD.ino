#include <GLCD_ST7565.h>
#include <JeeLib.h>
#include <EEPROM.h>

#include "utility/font_8x13B.h"
#include "utility/font_clR6x8.h"
#include "utility/font_clR6x6.h"
#include "utility/font_clR5x8.h"
#include "utility/font_5x7.h"
#include "utility/font_4x6.h"
#include "utility/font_lubB24.h"

// choose right utilization
#define PowerMeter 0
#define WeatherStation 1

#define NUMHIST 20

int send_id = 2; // shield emonTX
int cval_use;
double vrms;
double usekwh = 0;
unsigned long last_upd;

word dataHist [NUMHIST];

GLCD_ST7565 glcd;

//ISR(WDT_vect) { Sleepy::watchdogEvent(); }

typedef struct {
  int light;
  int humi;
  int temp;
  int pressure;
  byte lobat      :1;
  int battvol;
} Payload;
Payload measure;
/*
typedef struct {
  int power1;
  int power2;
  int power3;
  int power4;
  int Vrms;
} Payload;
Payload measure;
*/

void setup () {
  Serial.begin(9600);
  Serial.println("GLCD");
  /*
  #if PowerMeter
    rf12_initialize(1, RF12_433MHZ, 210);
  #endif
  #if WeatherStation
    rf12_initialize(1, RF12_433MHZ, 212);
  #endif
  */
  rf12_initialize(21, RF12_433MHZ, 210);

  glcd.begin();
  glcd.backLight(120);
  glcd.refresh();
  /*
  #if WeatherStation
    loadHist();
  #endif
  */
  showInfo();
  //debug();
}

void loop () {
  showInfo();
  if (rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof measure) {
    int node_id = (rf12_hdr & 0x1F);    // get node id
    Serial.println(node_id);
    if (node_id == send_id) {
      memcpy(&measure, (void*) rf12_data, sizeof measure);
      last_upd = millis();
      //calculation();
      //scroll();
      //debug();
      //showGraph();
      showInfo();
      //updEEPROM();
    }
  }
}

static void calculation() {
  #if PowerMeter
    cval_use = (measure.power1 + measure.power2 + measure.power3);
    usekwh += (cval_use * 0.2) / 3600000;
    dataHist[NUMHIST-1] = ((int)cval_use);
  #endif
  #if WeatherStation
    dataHist[NUMHIST-1] = ((int)measure.temp);
  #endif
}
/*
static void headerandfooter()
{
  char buf[10];
  // footer
  // last update
  glcd.setFont(font_4x6);
  glcd.drawString_P(5,  55, PSTR("next upd"));
  itoa(300-((millis()-last_upd)/1000), buf, 10);
  strcat(buf,"s");
  glcd.drawString(42, 55, buf);
  // header
  glcd.drawString_P(1,  0, PSTR("From node 2.0"));
  glcd.drawString_P(20, 12, PSTR("temp"));
  glcd.drawString_P(8, 20, PSTR("last 1.5h"));
}

static void scroll()
{
  for (byte i = 0; i < NUMHIST-1; ++i) {
    dataHist[i] = dataHist[i+1];
  }
}

static void loadHist()
{
  for (byte i = 0; i < NUMHIST-1; ++i)
  {
    dataHist[i] = EEPROM.read(i);
  }
}

static void updEEPROM()
{
  for (byte i = 0; i < NUMHIST-1; ++i)
  {
    EEPROM.write(i, dataHist[i]);
  }
}

static void debug()
{

  Serial.print("eeprom: ");
  for (byte i = 0; i < NUMHIST-1; ++i)
  {
    Serial.print(EEPROM.read(i));
    Serial.print(" ");
  }
  Serial.println(" ");

  Serial.print("table: ");
  for (byte i = 0; i < NUMHIST-1; ++i)
  {
    Serial.print(dataHist[i]);
    Serial.print(" ");
  }
  Serial.println(" ");
  #if PowerMeter
    Serial.print(measure.power1);
    Serial.print(" ");
    Serial.print(measure.power2);
    Serial.print(" ");
    Serial.print(measure.power3);
    Serial.println(" ");
  #endif
  #if WeatherStation
    Serial.print(measure.temp);
    Serial.print(" ");
    Serial.print(measure.humi);
    Serial.print(" ");
    Serial.print(measure.pressure);
    Serial.println(" ");
  #endif
}

static void showGraph()
{
  int maxValue = 0;
  // znajdz majwieksza wartosc
  for (byte i = 0; i < NUMHIST-1; ++i)
  {
    if (dataHist[i] > maxValue) maxValue = dataHist[i];
  }

  #if WeatherStation
    byte tick = 3;
    byte baseline = 50;
    glcd.drawLine(0, baseline, 58, baseline, 1);
  #endif
  #if PowerMeter
    word tick = maxValue + 55 / 56;
    if (tick < 1)
      tick = 1;
    byte baseline = 2 + maxValue / tick;
    glcd.drawLine(0, baseline, 58, baseline, 1);
  #endif

  for (byte i = 0; i < NUMHIST-1; ++i)
  {
    byte gHeight = dataHist[i] / tick;
    byte x = 3 * i + 1;
    #if WeatherStation
      glcd.fillRect(x, baseline - gHeight + 1, 2.5, gHeight, 1);
    #endif
    #if PowerMeter
      glcd.fillRect(x, baseline - gHeight + 1, 2.5, gHeight, 1);
    #endif
  }
}
*/
static void showInfo() {
  char buf[10];
  glcd.clear();
  #if PowerMeter
    glcd.setFont(font_4x6);
    glcd.drawString_P(0, 44, PSTR("P1 W"));
    glcd.drawString_P(32, 44, PSTR("P2 W"));
    glcd.drawString_P(62, 44, PSTR("P3 W"));
    glcd.drawString_P(92,  44, PSTR("Vrms V"));
    glcd.drawString_P(0,  0, PSTR("Razem"));

  #endif
  #if WeatherStation
    glcd.setFont(font_4x6);
    glcd.drawString_P(0, 44, PSTR("Vbat V"));
    glcd.drawString_P(42, 44, PSTR("Wilg %"));
    glcd.drawString_P(82, 44, PSTR("Cisn kHa"));
    glcd.drawString_P(0,  0, PSTR("Temperatura"));
  #endif

  glcd.setFont(font_clR6x8);
  #if PowerMeter
    itoa((int)measure.power1, buf, 10);
    glcd.drawString(0, 54, buf);
    itoa((int)measure.power2, buf, 10);
    glcd.drawString(32, 54, buf);
    itoa((int)measure.power3, buf, 10);
    glcd.drawString(62, 54, buf);
    dtostrf(measure.Vrms/100, 0, 0, buf);
    glcd.drawString(92, 54, buf);
    glcd.drawLine(0, 40, 128, 40, 1);

    //big
    glcd.setFont(font_lubB24);
    itoa((int)cval_use, buf, 10);
    strcat(buf," W");
    glcd.drawString(2, 8, buf);
  #endif
  #if WeatherStation
    dtostrf((float)measure.battvol/1000, 0, 1, buf);
    glcd.drawString(0, 54, buf);
    dtostrf((float)measure.humi/10, 0, 1, buf);
    glcd.drawString(42, 54, buf);
    dtostrf((float)measure.pressure/10, 0, 1, buf);
    glcd.drawString(82, 54, buf);
    glcd.drawLine(0, 40, 128, 40, 1);

    // big
    glcd.setFont(font_lubB24);
    dtostrf((float)measure.temp/10, 0, 1, buf);
    strcat(buf," *C");
    glcd.drawString(2, 8, buf);
  #endif
    /*
  #if WeatherStation
    headerandfooter();
  #endif
  */
  //showGraph();
  glcd.refresh();
}
