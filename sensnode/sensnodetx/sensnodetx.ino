/*
SensnodeTX v3.4-dev
Written by Artur Wronowski <artur.wronowski@digi-led.pl>
Need Arduino 1.0 to compile

TODO:
- pomiar napiecia bateri, skalowanie czasu pomiedzy pomiarami w zaleznosci od panujacego napiecia
- srednia z pomiarow
- wiartomierz //#define ObwAnem 0.25434 // meters
- http://hacking.majenko.co.uk/node/57
*/

// libs for I2C and DS18B20
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// lisb for SHT21 and BMP085
#include <BMP085.h>
#include <SHT2x.h>
//#include <Adafruit_BMP085.h>
// lib for RFM12B from https://github.com/jcw/jeelib
#include <JeeLib.h>
// avr sleep instructions
#include <avr/sleep.h>
// DHT11 and DHT22 from https://github.com/adafruit/DHT-sensor-library
#include "DHT.h"

/*************************************************************/

#define NODEID              15
#define NETWORK             210
#define NEW_REV                 // comment for old revision  (3.0 or 3.4)

// Settings
//#define SMOOTH            3   // smoothing factor used for running averages
#define PERIOD              1   // minutes
#define RADIO_SYNC_MODE     2   // http://jeelabs.net/pub/docs/jeelib/RF12_8h.html#a6843bbc70df373dbffa0b3d1f33ef0ae

// Used devices (comment on uncomment if not used)
#define LDR                    // use LDR sensor
#define DS18B20                // use 1WIE DS18B20

#define SHT21_SENSOR           // use SHT21
#define BMP_SENSOR             // use BMP085 or BMP180
#define DHT_SENSOR             // DHT11 or DHT22


#define LED_ON                 // use act led for transmission

#define DEBUG                  // debug mode - serial output

/*************************************************************/

#ifdef DS18B20
  #define DS_COUNT          3  // http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html , http://forum.arduino.cc/index.php?topic=143382.0
#endif

#ifdef SHT21_SENSOR || BMP_SENSOR
  #define I2C                  // use i2c bus for BMP085/BMP180 and SHT21
#endif

#ifdef DHT_SENSOR
  #define DHTTYPE           DHT22
  #define DHTPIN            4  // port P1 = digital 4
#endif

#ifndef NEW_REV
  #undef LDR                  // disable LDR on old hardware 3.0
#endif

/*************************************************************/

// Input/Output definition
// Analog
#ifdef NEW_REV //3.4
  #define LDR               2
  #define BAT_VOL           3
#else //3.0
  #define LDR               0
  #define BAT_VOL           1
  #define CustomA3          3
#endif

// Digital
#ifdef NEW_REV //3.4
  #define MOSFET            7
  #define ONEWIRE_DATA      8
  #define ACT_LED           9
#else //3.0
  #define CustomD3          3
  #define CustomD4          4
  #define MOSFET            5
  #define ONEWIRE_DATA      8
  #define ACT_LED           9
#endif

/************************************************************/

// structure of received data

typedef struct {
  int light;
  int humi;
  int temp;
  int pressure;
  byte lobat      :1;
  int battvol;
} Payload;
Payload measure;

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

#ifdef NEW_REV
  //Port p1 (1); // JeeLabs Port P1
  Port p2 (2); // JeeLabs Port P2
  Port ldr (3);  // Analog pin 2
  Port batvol (4); // Analog pin 3
#endif

byte count = 0;
int tempReading;

#ifdef DS18B20
    OneWire oneWire(ONEWIRE_DATA);
    DallasTemperature sensors(&oneWire);
#endif

#ifdef DHT_SENSOR
  DHT dht(DHTPIN, DHTTYPE);
#endif

void setup()
{
    rf12_initialize(NODEID, RF12_433MHZ, NETWORK);
    rf12_control(0xC040); // 2.2v low

#ifdef DHT_SENSOR
  dht.begin();
#endif

#ifdef I2C
    Wire.begin();
#endif

#ifdef DEBUG
    Serial.begin(115200);
#endif

#if ONEWIRE
    sensors.begin();
#endif

}

void loop()
{
  doMeasure(); // mierz
  #ifdef DEBUG
     transmissionRS();
  #endif
  #ifdef LED_ON
    activityLed(1);
  #endif
  doReport(); // wyslij
  #ifdef LED_ON
    activityLed(0);
  #endif

  for (byte t = 0; t < PERIOD; t++)  // spij
    Sleepy::loseSomeTime(60000);
}

static void doReport()
{
  rf12_sleep(RF12_SLEEP);
  while (!rf12_canSend())
    rf12_recvDone();
  rf12_sendStart(0, &measure, sizeof measure);
  rf12_sendWait(RADIO_SYNC_MODE);
  rf12_sleep(RF12_SLEEP);
}

static void transmissionRS()
{
  activityLed(1);
  Serial.println(' ');
  Serial.print("LIGHT ");
  Serial.println(measure.light);
  Serial.print("HUMI ");
  Serial.println(measure.humi);
  Serial.print("TEMP ");
  Serial.println(measure.temp);
  Serial.print("PRES ");
  Serial.println(measure.pressure);
  Serial.print("LOBAT " );
  Serial.println(measure.lobat, DEC);
  Serial.print("BATVOL ");
  Serial.println(measure.battvol);
  /*
  Serial.print("VCCREF ");
  Serial.println(readVcc());
  */
  activityLed(0);
}

static void activityLed (byte on) {
  pinMode(ACT_LED, OUTPUT);
  digitalWrite(ACT_LED, on);
  delay(150);
}
/*
 long readVcc() {
   long result;
   // Read 1.1V reference against Vcc
   ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
   delay(2); // Wait for Vref to settle
   ADCSRA |= _BV(ADSC); // Convert
   while (bit_is_set(ADCSRA,ADSC));
   result = ADCL;
   result |= ADCH<<8;
   result = 1126400L / result; // Back-calculate Vcc in mV
   return result;
   ADCSRA &= ~ bit(ADEN); bitSet(PRR, PRADC); // Disable the ADC to save power
}
*/
static void doMeasure() {
  count++;
  tempReading = 0;

#ifdef NEW_REV
  for (byte t = 0;t < 3; t++) {
    tempReading += batvol.anaRead();
    Sleepy::loseSomeTime(50);
  }
  tempReading = tempReading / 3;
  measure.battvol = map(tempReading,0,1023,0,6600);
#else
  for (byte t = 0;t < 3; t++) {
    tempReading += analogRead(BAT_VOL);
    Sleepy::loseSomeTime(50);
  }
  tempReading = tempReading / 3;
  measure.battvol = map(tempReading,0,1023,0,6600);
#endif

//  measure.battvol = readVcc();
  Serial.print(".");
  //measure.nodeid = NODEID;
  measure.lobat = rf12_lowbat();

#ifdef LDR
  if ((count % 2) == 0) {
     measure.light = ldr.anaRead();
  }
  /*
    def get_light_level(self):
    result = self.adc.readADC(self.adcPin) + 1
    vout = float(result)/1023 * 3.3
    rs = ((3.3 - vout) / vout) * 5.6
    return abs(rs)
    */
#endif

#ifdef I2C
  float shthumi = SHT2x.GetHumidity();
  float shttemp = SHT2x.GetTemperature();
  measure.humi = shthumi * 10;
  measure.temp = shttemp * 10;
  Sleepy::loseSomeTime(250);
  BMP085.getCalData();
  BMP085.readSensor();
  measure.pressure = (BMP085.press*10*10) + 16;
#endif

#ifdef DS18B20
  sensors.requestTemperatures();
  Sleepy::loseSomeTime(750);
  float tmp = sensors.getTempCByIndex(0);
  measure.temp = tmp * 10;
#endif

#ifdef DHT_SENSOR
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else {
    measure.humi = h*10;
    measure.temp = t*10;
  }
#endif
}

