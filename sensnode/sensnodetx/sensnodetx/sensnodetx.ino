#include <Arduino.h>

 /*
SensnodeTX v4.1-dev
Written by Artur Wronowski <artur.wronowski@digi-led.pl>
Bassed od Jeenode projekt by Jean-Claude Wippler <jc@wippler.nl>

TODO:
- wiartomierz //#define ObwAnem 0.25434 // meters
*/

#include "nodes.h"
#include "configuration.h"
#include "profiles.h"

#if RFM69 == 1
   #define RF69_COMPAT 1
#else
   #define RF69_COMPAT 0
#endif

#include <Wire.h>
#if defined DS18B20
   #include <OneWire.h>
   #include <DallasTemperature.h>
#endif
// lib for SHT21 and BMP085
#if defined SHT21_SENSOR || defined BMP_SENSOR 
   #include <BMP085.h>
   #include <SHT2x.h>
#endif
// lib for RFM12B from https://github.com/jcw/jeelib
#include <JeeLib.h>
// avr sleep instructions
#include <avr/sleep.h>
// DHT11 and DHT22 from https://github.com/adafruit/DHT-sensor-library
#ifdef DHT_SENSOR
   #include "DHT.h"
#endif
#ifdef OLED
   #include <OzOLED.h>
   #include <stdlib.h>
#endif
// EEPROM needed for relay states
#include <EEPROM.h>
#include "boards.h"

/*************************************************************/

#ifdef DS18B20
  byte ds_array[DS_COUNT];
#endif

#ifdef DHT_SENSOR
  float h; // define humidity DTH variable
  float t; //define temperature DTH variable
  #define DHTTYPE           DHT_SENSOR_TYPE
  #define DHTPIN            4  // port P1 = digital 4
#endif

#if defined SHT21_SENSOR || defined BMP_SENSOR
  #define I2C
#endif

#ifdef DEV_MODE
  #define DEBUG
#endif

#ifndef DEBUG_BAUD
  #define DEBUG_BAUD        9600
#endif

/*************************************************************/

byte count = 0;
unsigned int adcreading;
byte numberOfDevices;
int volts;
byte receivedOK = 0;
byte relay_eeprom_addr = 0;

char buf[10];

// structure of sended data

typedef struct {
  int light;
  int humi;
  #if defined DS18B20 && defined DS_COUNT > 1
    int temp0;
    int temp1;
    int temp2;
  #else
    int temp;
  #endif
  int pressure;
  int battvol;
#ifdef AIRQ
  int co2;
#endif
#ifdef LPG
  int lpg;
#endif
#ifdef RELAY
  byte relay :1;
#endif
} PayloadTX;
PayloadTX measure;

typedef struct {
    byte destnode;
    byte cmd;
    byte state :1;
} PayloadRX;
PayloadRX rxdata;

ISR(WDT_vect) { Sleepy::watchdogEvent(); }


#ifdef DS18B20
  #define TEMPERATURE_PRECISION 9
  OneWire oneWire(ONEWIRE_DATA);
  DallasTemperature sensors(&oneWire);
  DeviceAddress tempDeviceAddress;
#endif

#ifdef DHT_SENSOR
  DHT dht(DHTPIN, DHTTYPE);
#endif


#ifdef RELAY
  #define RELAY_PIN 1
  Port relay (RELAY_PIN); //P1
#endif


#ifdef LPG
  #define LPG_PIN 2
  Port lpg (LPG_PIN);    //P2
#endif

#ifdef AIRQ
  #define AIRQ_PIN 3
  Port air (AIRQ_PIN);  //P3
#endif

void setup()
{
  #if BAND == 433
    rf12_initialize(NODEID, RF12_433MHZ, NETWORK);
  #endif
  #if BAND == 868
    rf12_initialize(NODEID, RF12_868MHZ, NETWORK);
  #endif
  #if defined LOWRATE & defined RFM69 == 0
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

#ifdef AIRQ
  air.mode(INPUT);
#endif

#ifdef RELAY
  relay.mode(OUTPUT);
#endif

#ifdef LPG
  lpg.mode(INPUT);
#endif

#ifdef OLED
  OzOled.init();
  OzOled.clearDisplay();
  OzOled.setPageMode();
#endif
}


void doReport()
{
  rf12_sleep(RF12_WAKEUP);
  while (!rf12_canSend())
    rf12_recvDone();
  rf12_sendNow(0, &measure, sizeof measure);
  rf12_sendWait(RADIO_SYNC_MODE);
  rf12_sleep(RF12_SLEEP);
}


void doReceive() {
  if (rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof rxdata) {
    #ifdef LED_ON
      recvLED();
    #endif
    memcpy(&rxdata, (void*) rf12_data, sizeof rxdata);
    if(RF12_WANTS_ACK){
      rf12_sendStart(RF12_ACK_REPLY,0,0);
    }
  }

  if (rxdata.destnode == NODEID) {
    // Custom commands
    // eg. ON/OFF SSR Relay
    if (rxdata.cmd == 1) {
      if (rxdata.state == 1) {
        receivedOK = 1;
        EEPROM.write(relay_eeprom_addr, rxdata.state);
        relay.digiWrite(1);
      }
      else {
        receivedOK = 0;
        EEPROM.write(relay_eeprom_addr, rxdata.state);
        relay.digiWrite(0);

      }
    }
  }
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
  #if defined(__AVR_ATmega168__) 
    analogReference(DEFAULT);
  #else
    analogReference(INTERNAL);
  #endif
  double vccref = readVcc()/1000.0;
  #if BOARD_REV == 4
    float resistor_tolerance = 1.06; // 1%
    // R6 = 1M %1, R7 = 10M %1
	adcreading = analogRead(BAT_VOL) * 10 * resistor_tolerance;
  #else
  // R6 = 1M %1, R7 = 1M %1
	adcreading = analogRead(BAT_VOL) * 2;
  #endif
  double battvol = (adcreading / 1023.0) * vccref;
  return battvol * 1000;
}


void sendLED(byte on) {
  pinMode(ACT_LED, OUTPUT);
  digitalWrite(ACT_LED, on);
  delay(100);
}


void recvLED() {
  pinMode(ACT_LED, OUTPUT);
  digitalWrite(ACT_LED, HIGH);
  delay(50);
  digitalWrite(ACT_LED, LOW);
  delay(100);
  digitalWrite(ACT_LED, HIGH);
  delay(50);
  digitalWrite(ACT_LED, LOW);
}


void transmissionRS()
{
  #ifdef DEV_MODE
    Serial.println("==DEV MODE==");
    Serial.print("NODEID: ");
    Serial.print(NODEID);
    delay(2);
  #endif
  //sendLED(1);
  Serial.println(' ');
  delay(2);
  #ifdef LDR_SENSOR
    Serial.print("LIGHT ");
    Serial.println(measure.light);
    delay(2);
  #endif
  Serial.print("HUMI ");
  Serial.println(measure.humi);
  delay(2);
  #if defined DS18B20 || defined SHT21_SENSOR || defined DHT_SENSOR
    #if defined DS_COUNT && defined DS_COUNT > 1
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
  #ifdef AIRQ
    Serial.print("CO2 ");
    Serial.println(measure.co2);
  #endif
  delay(2);
  #ifdef LPG
    Serial.print("LPG ");
    Serial.println(measure.lpg);
  #endif
  Serial.print("BATVOL ");
  Serial.println(measure.battvol);
  delay(2);
  Serial.println(' ');
  Serial.print("RECEIVED ");
  Serial.println(EEPROM.read(0));
  Serial.println(' ');
  delay(2);
  //sendLED(0);
}


void doMeasure() {
  count++;

#if defined FAKE_BAT
  measure.battvol = 5000;
#else
  measure.battvol = battVolts();
#endif
  
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

#ifdef AIRQ
  byte i = 0;
  float val = 0;
  for(i=0;i<20;i++) {
    val += air.anaRead();
    delay(100);
  }
  measure.co2 = (val/20.0) * 10;
#endif

#ifdef LPG
  byte i = 0;
  float val = 0;
  for(i=0;i<20;i++) {
    val += lpg.anaRead();
    delay(100);
  }
  measure.lpg = (val/20.0) * 10;
#endif

#ifdef RELAY
  measure.relay = EEPROM.read(relay_eeprom_addr);
#endif
}


void doDisplay() {
#ifdef OLED
  OzOled.setCursorXY(0,0);
  OzOled.printString("-=");
  OzOled.setCursorXY(4,0);
  OzOled.printString(NODENAME);
  OzOled.setCursorXY(14,0);
  OzOled.printString("=-");

  OzOled.setCursorXY(0,4);
  OzOled.printString("BAT:");
  OzOled.setCursorXY(6,4);
  OzOled.printNumber((float)measure.battvol/1000, 2);
  OzOled.setCursorXY(13,4);
  OzOled.printString("V");
/*
  OzOled.setCursorXY(0,6);
  OzOled.printString("LUPD:");
  OzOled.setCursorXY(6,6);
  OzOled.printNumber((float)(m * 60)+s);
  OzOled.setCursorXY(13,6);
  OzOled.printString("s");
*/
  #ifdef DS18B20
    OzOled.setCursorXY(0,2);
    OzOled.printString("TEMP:");
    OzOled.setCursorXY(6,2);
    OzOled.printNumber((float)measure.temp/10, 2);
    OzOled.setCursorXY(13,2);
    OzOled.printString("*C");
  #endif

  #ifdef AIRQ
    OzOled.setCursorXY(0,6);
    OzOled.printString("AIRQ:");
    OzOled.setCursorXY(6,6);
    OzOled.printNumber((float)measure.co2/10, 2);
    OzOled.setCursorXY(13,6);
    OzOled.printString("ppm");
  #endif

#endif
}


void sendPayload()
{
  doMeasure(); // measure
  #ifdef DEBUG
    transmissionRS();
  #endif
  #ifdef LED_ON
    sendLED(1);
  #endif
  #ifndef DEV_MODE
    doReport(); // send
  #endif
  #ifdef LED_ON
    sendLED(0);
  #endif
  #ifdef OLED
    doDisplay();
  #endif
}


// Main loop
void loop() {
  int vcc;
  
#if defined FAKE_BAT
  vcc = 5000;
#else
  vcc = battVolts();
#endif

#ifdef RELAY
  relay.digiWrite(EEPROM.read(0));
#endif
 
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
      //Sleepy::loseSomeTime(60000);
      for (int i = 0; i < 60000/70; ++i) {
        Sleepy::loseSomeTime(10);
        doReceive();
      }
    #else
      //Sleepy::loseSomeTime(6000);
      for (int i = 0; i < 6000/70; ++i) {
        Sleepy::loseSomeTime(10);
        doReceive();
      }
    #endif
}
