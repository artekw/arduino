/*
SensnodeTX v3.4-dev
Written by Artur Wronowski <artur.wronowski@digi-led.pl>
Need Arduino 1.0 to compile

TODO:
- srednia z pomiarow
- wiartomierz //#define ObwAnem 0.25434 // meters
- http://hacking.majenko.co.uk/node/57
*/

#include "configuration.h"
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

#ifdef DS18B20
  int ds_array[DS_COUNT];
#endif

#ifdef DHT_SENSOR
  float h; // define humidity DTH variable
  float t; //define temperature DTH variable
  #define DHTTYPE           DHT_SENSOR_TYPE
  #define DHTPIN            4  // port P1 = digital 4
#endif

#ifdef SHT21_SENSOR || BMP_SENSOR
  #define I2C
#endif

#ifdef DEV_MODE
  #define DEBUG
#endif

#ifndef DEBUG_BAUD
  #define DEBUG_BAUD        9600
#endif

/*************************************************************/

// Input/Output definition
// Analog
#ifdef NEW_REV //3.4
  #define LDR_PIN           2
  #define BAT_VOL           3
#else //3.0
  #define LDR_PIN           0
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

byte count = 0;
unsigned int adcreading;
int numberOfDevices;
int volts;


// structure of sended data

typedef struct {
  int light;
  int humi;
  #ifdef DS18B20 && DS_COUNT > 1
    int temp0;
    int temp1;
    int temp2;
  #else
    int temp;
  #endif
  int pressure;
  byte lobat      :1;
  int battvol;
} Payload;
Payload measure;

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

#ifdef NEW_REV
  Port p1 (1); // JeeLabs Port P1
  Port p2 (2); // JeeLabs Port P2
  Port batvol (3);
#endif

#ifdef DS18B20
    #define TEMPERATURE_PRECISION 9
    OneWire oneWire(ONEWIRE_DATA);
    DallasTemperature sensors(&oneWire);
    DeviceAddress tempDeviceAddress;
#endif

#ifdef DHT_SENSOR
  DHT dht(DHTPIN, DHTTYPE);
#endif


void setup()
{
  #if BAND == 433
    rf12_initialize(NODEID, RF12_433MHZ, NETWORK);
  #endif
  #if BAND == 868
    rf12_initialize(NODEID, RF12_868MHZ, NETWORK);
  #endif
  rf12_control(0xC040); // 2.2v low
  #if LOWRATE
    rf12_control(0xC623); // ~9.6kbps
  #endif

#ifdef DHT_SENSOR
  dht.begin();
#endif

#ifdef I2C
    Wire.begin();
#endif

#ifdef DEBUG
    Serial.begin(DEBUG_BAUD);
#endif

#ifdef DS18B20
    sensors.begin();
    numberOfDevices = sensors.getDeviceCount();
#endif

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


long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1125300L / result;
  return result;
}


int battVolts(void) {
  adcreading=analogRead(BAT_VOL) * 2;
  double vccref = readVcc()/1000.0;
  double battvol = (adcreading / 1023.0) * vccref;
  return battvol * 1000;
}


static void activityLed (byte on) {
  pinMode(ACT_LED, OUTPUT);
  digitalWrite(ACT_LED, on);
  delay(150);
}


static void transmissionRS()
{
  #ifdef DEV_MODE
    Serial.println("==DEV MODE==");
    delay(2);
  #endif
  activityLed(1);
  Serial.println(' ');
  delay(2);
  #ifdef LDR
    Serial.print("LIGHT ");
    Serial.println(measure.light);
    delay(2);
  #endif
  Serial.print("HUMI ");
  Serial.println(measure.humi);
  delay(2);
  #ifdef DS18B20 || SHT21_SENSOR || DHT_SENSOR
    #ifdef DS_COUNT && DS_COUNT > 1
      for (byte i=0; i < DS_COUNT; i++) { 
        Serial.print("TEMP");
        Serial.print(i);
        Serial.print(" ");
        Serial.println(ds_array[i]);
      }
    #else
      Serial.print("TEMP ");
      Serial.println(measure.temp);
    #endif
    #else
      Serial.print("TEMP ");
      Serial.println(measure.temp);
  #endif
  Serial.print("PRES ");
  Serial.println(measure.pressure);
  delay(2);
  Serial.print("LOBAT " );
  Serial.println(measure.lobat, DEC);
  delay(2);
  Serial.print("BATVOL ");
  Serial.println(measure.battvol);
  delay(2);
  Serial.println(' ');
  delay(2);
  activityLed(0);
}


static void doMeasure() {
  count++;
  adcreading = 0;

  measure.lobat = rf12_lowbat();
  measure.battvol = battVolts();
  
#ifdef LDR_SENSOR
  if ((count % 2) == 0) {
     measure.light = analogRead(LDR_PIN);
  }
#endif

#ifdef I2C
 #ifdef SHT21_SENSOR
   float shthumi = SHT2x.GetHumidity();
   measure.humi = shthumi * 10;
   #ifndef DS18B20
     float shttemp = SHT2x.GetTemperature();
     measure.temp = shttemp * 10;
   #endif
 #endif

 #ifdef BMP_SENSOR 
  Sleepy::loseSomeTime(250);
  BMP085.getCalData();
  BMP085.readSensor();
  measure.pressure = ((BMP085.press*10*10) + 16);
 #endif
#endif

#ifdef DS18B20
  #ifdef DEBUG
    Serial.print("DS18B20 found: ");
    Serial.println(numberOfDevices, DEC);
  #endif

  #if DS_COUNT > 1  // TODO
    for(byte i=0; i < DS_COUNT; i++) {
      sensors.requestTemperatures();
      Sleepy::loseSomeTime(750);
      float tmp = sensors.getTempCByIndex(i);
      ds_array[i] = tmp * 10;
    }
    measure.temp0 = ds_array[0];
    measure.temp1 = ds_array[1];
    measure.temp2 = ds_array[2];
  #else
    sensors.requestTemperatures();
    Sleepy::loseSomeTime(750);
    float tmp = sensors.getTempCByIndex(0);
    measure.temp = tmp * 10;
  #endif
#endif


#ifdef DHT_SENSOR
 #ifndef SHT21
   float h = dht.readHumidity();
   #ifndef DS18B20
     float t = dht.readTemperature();
   #endif
   if (isnan(h) || isnan(t)) {
     return;
   } 
   else {
     measure.humi = h*10;
     #ifndef DS18B20
       measure.temp = t*10;
     #endif
   }
  #endif
#endif
}

static void sendPayload()
{
  doMeasure(); // measure
  #ifdef DEBUG
     transmissionRS();
  #endif
  #ifdef LED_ON
    activityLed(1);
  #endif
  #ifndef DEV_MODE
    doReport(); // send
  #endif
  #ifdef LED_ON
    activityLed(0);
  #endif
}

// Main loop
void loop() {
  int vcc = battVolts();
  
  if (vcc <= VCC_FINAL) { // ostatni mozliwy pakiet
    sendPayload();
    vcc = 1; // don't even try reading VCC after this send
  }

  if (vcc >= VCC_OK) { // wszytko ok
    sendPayload();
  }

  int minutes = VCC_SLEEP_MINS(vcc);
  while (minutes-- > 0)
    #ifndef DEV_MODE
      Sleepy::loseSomeTime(60000);
    #else
      Sleepy::loseSomeTime(6000);
    #endif

}

