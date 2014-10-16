/*
SensnodeTX v3.4-dev
Written by Artur Wronowski <artur.wronowski@digi-led.pl>
Documentation: http://lab.digi-led.pl/wiki/doku.php/sensnodetx
Works with optiboot too.
Need Arduino 1.0 to compile


TODO:
- srednia z pomiarow
- wiartomierz //#define ObwAnem 0.25434 // meters
- Making accurate ADC readings on the Arduino http://hacking.majenko.co.uk/node/57
*/

// libs for I2C and DS18B20
#include <dht11.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// lisb for SHT21 and BMP085
#include <BMP085.h>
#include <SHT2x.h>

// lib for RFM12B from https://github.com/jcw/jeelib
#include <JeeLib.h>
// DHT11 Sensor

// avr sleep instructions
#include <avr/sleep.h>

/*************************************************************/
// Network settings
#define myNodeID            19          // Node ID
#define network             210         // 
#define band                RF12_433MHZ // Band

// Hardware settings
#define NEW_REV             1       // sensnodeTx Revision: 3.0 or 3.4; Use 1 if don't know

// Transmission settings
#define RADIO_SYNC_MODE     2       // http://jeelabs.net/pub/docs/jeelib/RF12_8h.html#a6843bbc70df373dbffa0b3d1f33ef0ae
#define LOWRATE             0       // force 9.6kbps for better reception of radio http://forum.jeelabs.net/node/1086 ; All node need to use the same datarate!

// Used devices or buses (1 on 0)
#define LDR                 0       // use LDR sensor
#define DS18B20             0       // use 1WIRE DS18B20
#define I2C                 1       // use i2c bus for BMP085 and SHT21
#define DEBUG               0       // debug mode - serial output
#define LED_ON              0       // use act led for transmission, faster battery drain
#define DHT11               0       // DHT11 Sensor; Use P1 port

// Additional settings for devices
#define DHT11PIN            4      // DHT11 Sensor use P1 port = D4
/*************************************************************/
/********************DON'T EDIT COD BELOW*********************/
/*************************************************************/

// Input/Output definition
// Analog
#if NEW_REV //3.4
  #define LDR               2
  #define BAT_VOL           3
#else //3.0
  #define LDR               0
  #define BAT_VOL           1
  #define CustomA3          3
#endif

// Digital
#if NEW_REV //3.4
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

volatile bool adcDone;

ISR(ADC_vect) { adcDone = true; }
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

#if NEW_REV
  // Port p1 (1); // JeeLabs Port P1
  // Port p2 (2); // JeeLabs Port P2
  Port ldr (3);  // Analog pin 2
  Port batvol (4); // Analog pin 3
#endif

byte count = 0;

#if DS18B20
  OneWire oneWire(ONEWIRE_DATA);
  DallasTemperature sensors(&oneWire);
#endif

#if DHT11
  dht11 DHT;
#endif

void setup() {
  #if DEBUG
      Serial.begin(9600);
  #endif
 
  
  rf12_initialize(myNodeID, band, network);
  rf12_control(0xC040); // 2.2v low
  #if LOWRATE
    rf12_control(0xC623); // ~9.6kbps
  #endif

  #if I2C
    Wire.begin();
  #endif
        
/*
  #if ONEWIRE
      sensors.begin();
  #endif
*/
}
static void sendPayload()
{
  doMeasure(); // mierz

  #if DEBUG
     transmissionRS();
  #endif
  #if LED_ON
    activityLed(1);
  #endif
  doReport(); // wyslij
  #if LED_ON
    activityLed(0);
  #endif
}

static void doReport() {
  rf12_sleep(RF12_SLEEP);
  while (!rf12_canSend())
    rf12_recvDone();
  rf12_sendStart(0, &measure, sizeof measure);
  rf12_sendWait(RADIO_SYNC_MODE);
  rf12_sleep(RF12_SLEEP);
}

static byte vccRead (byte count =4) {
  set_sleep_mode(SLEEP_MODE_ADC);
  ADMUX = bit(REFS0) | 14; // use VCC and internal bandgap
  bitSet(ADCSRA, ADIE);
  while (count-- > 0) {
    adcDone = false;
    while (!adcDone)
      sleep_mode();
  }
  bitClear(ADCSRA, ADIE);
  // convert ADC readings to fit in one byte, i.e. 20 mV steps:
  //  1.0V = 0, 1.8V = 40, 3.3V = 115, 5.0V = 200, 6.0V = 250
  return (55U * 1023U) / (ADC + 1) - 50;
}

static void transmissionRS() {
  activityLed(1);
  Serial.println(' ');
  Serial.print("LIGHT ");
  Serial.println(measure.light);
  Serial.print("HUMI ");
  Serial.println(measure.humi);
  Serial.print("TEMP ");
  Serial.println(measure.temp);
  Serial.print("PRESS ");
  Serial.println(measure.pressure);
  Serial.print("LOBAT " );
  Serial.println(measure.lobat, DEC);
  Serial.print("BATVOL ");
  Serial.println(measure.battvol);
  Serial.print("VCCREF ");
  Serial.println(vccRead());
  activityLed(0);
}

static void activityLed (byte on) {
  pinMode(ACT_LED, OUTPUT);
  digitalWrite(ACT_LED, on);
  delay(150);
}

static void doMeasure() {
  count++;
  int tempReading = 0;

  //Serial.print(".");
  measure.lobat = rf12_lowbat();

#if NEW_REV
  for (byte t = 0;t < 3; t++) {
    tempReading += batvol.anaRead();
    Sleepy::loseSomeTime(50);
  }
  tempReading = tempReading / 3;
  measure.battvol = map(tempReading,0,1023,0,6600);
  #if LDR
    measure.light = map(ldr.anaRead(),0,1023,100,0);
  #endif
#else
  for (byte t = 0;t < 3; t++) {
    tempReading += analogRead(BAT_VOL);
    Sleepy::loseSomeTime(50);
  }
  tempReading = tempReading / 3;
  measure.battvol = map(tempReading,0,1023,0,6600);
  #if LDR
    measure.light = map(analogRead(LDR),0,1023,100,0);
  #endif
#endif

#if DHT11
  DHT.read(DHT11PIN);
  float h = DHT.humidity;
  float t = DHT.temperature;
  if (isnan(t) || isnan(h)) {
    return;
  }
  else {
    measure.temp = t*10;
    measure.humi = h*10;
  }
#endif

#if I2C
  float shthumi = SHT2x.GetHumidity();
  float shttemp = SHT2x.GetTemperature();
  measure.humi = shthumi * 10;
  measure.temp = shttemp * 10;
  Sleepy::loseSomeTime(250);
  BMP085.getCalData();
  BMP085.readSensor();
  measure.pressure = (BMP085.press*10*10) + 16;
#endif

#if DS18B20
  sensors.requestTemperatures();
  Sleepy::loseSomeTime(750);
  float tmp = sensors.getTempCByIndex(0);
  measure.temp = tmp * 10;
#endif  
}

// https://github.com/jcw/jeelib/blob/master/examples/RF12/radioBlip2/radioBlip2.ino
#define VCC_OK    85  // >= 2.7V - enough power for normal 1-minute sends
#define VCC_LOW   80  // >= 2.6V - sleep for 1 minute, then try again
#define VCC_DOZE  75  // >= 2.5V - sleep for 5 minutes, then try again
                      //  < 2.5V - sleep for 60 minutes, then try again
#define VCC_SLEEP_MINS(x) ((x) >= VCC_LOW ? 1 : (x) >= VCC_DOZE ? 5 : 60)

#define VCC_FINAL 70  // <= 2.4V - send anyway, might be our last swan song

void loop() {
  byte vcc = vccRead();

  if (vcc <= VCC_FINAL) { // hopeless, maybe we can get one last packet out
    sendPayload();
    vcc = 1; // don't even try reading VCC after this send
  }

  if (vcc >= VCC_OK) { // enough energy for normal operation
    sendPayload();
  }

  byte minutes = VCC_SLEEP_MINS(vcc);
  while (minutes-- > 0)
    Sleepy::loseSomeTime(60000);
}
