/*
SensnodeTX v1.0-dev
Written by Artur Wronowski <artur.wronowski@digi-led.pl>
Works with optiboot too.
Need Arduino 1.0-RC1 do compile
*/

// libs for I2C and 1Wire
#include <Wire.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>
// lisb for SHT21 and BMP085
#include <SHT21.h>
#include <BMP085.h>
// lib for RFM12B from <jeelabs.org>
#include <JeeLib.h>
// avr sleep instructions
#include <avr/sleep.h>

/*************************************************************/

static byte NODEID        = 2;  // ID this node
static byte NODEGROUP     = 212;

// Structure of Data
typedef struct {
	int nodeid;
	int light;
	float humi;
	float temp;
	float pressure;
	byte lobat		:1;
	int battvol;
	int solvol;
	byte solar		:1;
} Payload;
Payload measure;

// Input/Output definition
// Analog
#define LDRPin            0
#define WindPin           2
#define BatteryVolPin     3

// Digital
#define ACT_LED           3
#define ONEWIRE_DATA      4
#define MOSFET_SOL        5

// Settings
#define MEASURE_PERIOD    5 //300
#define RETRY_ACK         3  //? // how many times try get ack
#define REPORT_EVERY      10 //20  
#define ACK_TIME          100 //? // time for recive ack packet (in milisec)
#define RETRY_PERIOD      10 //?
#define LDR_TR            50 // LDR treshold, over activate solar

//#define ObwAnem 0.25434 // meters

// Used devices and buses
#define LDR               0
#define ONEWIRE           0 // use 1wire bus
#define I2C               1 // use i2c bus
#define SHT21_TEMP        0 // read temperature from SHT21 (only if I2C bus is used) not implemented yet
#define DEBUG             1 // debug mode - serial
#define LED_ON            0 // use act led - transmission
#define SOLAR             0 // use solar to power device and charge batteries
#define NTC               0 // wind measure

#define RADIO_SYNC_MODE   2

#define rf12_sleep(x)

/************************************************************/

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

enum { MEASURE, REPORT, TASKS };

static word schedbuf[TASKS];
Scheduler scheduler (schedbuf, TASKS);

static byte reportCount;

#if ONEWIRE
    OneWire oneWire(ONEWIRE_DATA);
    DallasTemperature sensors(&oneWire);
#endif

void setup()
{
    rf12_initialize(NODEID, RF12_433MHZ, NODEGROUP);

#if I2C
    Wire.begin();
    BMP085.getCalData();
#endif

#if DEBUG
    Serial.begin(115200);
#endif

#if ONEWIRE
    sensors.begin();
#endif

  rf12_sleep(0);

  reportCount = REPORT_EVERY;
  scheduler.timer(MEASURE, 0);
}

void loop()
{
  #if DEBUG
     Serial.print('.');
     delay(2);	
  #endif
  switch (scheduler.pollWaiting()) {

    case MEASURE:
        scheduler.timer(MEASURE, MEASURE_PERIOD);
        doMeasure();

        if (++reportCount >= REPORT_EVERY) {
          reportCount = 0;
          scheduler.timer(REPORT, 0);
        }
        break;
    case REPORT:
        if (DEBUG) {
          transmissionRS();
          doReport2();
        }
        else
        {
          doReport2();
        }
        break;
  }
}

static void doReport()
{
  for (byte i = 0; i < RETRY_ACK; ++i) {
    rf12_sleep(-1);
    while (!rf12_canSend())
      rf12_recvDone();
    rf12_sendStart(RF12_HDR_ACK, &measure, sizeof measure);
    rf12_sendWait(RADIO_SYNC_MODE);
    byte acked = waitForACK();
    rf12_sleep(0);
    
    if (acked) {
      #if DEBUG
        Serial.print("ACK! ");
        Serial.println((int) i);
        Serial.println("-------");
        delay(2);
      #endif
      scheduler.timer(MEASURE, MEASURE_PERIOD);
      return;
    }
    Sleepy::loseSomeTime(RETRY_PERIOD * 100);
  }
  scheduler.timer(MEASURE, MEASURE_PERIOD);
  #if DEBUG
    Serial.print("no ACK! ");
    Serial.println("-------");
    delay(2);
  #endif
}

static void doReport2()
{
  rf12_sleep(-1);
  while (!rf12_canSend())
    rf12_recvDone();
  rf12_sendStart(0, &measure, sizeof measure);
  rf12_sendWait(RADIO_SYNC_MODE);
  rf12_sleep(0);
}

static byte waitForACK() {
  MilliTimer t;
  while (!t.poll(ACK_TIME)) {
    if (rf12_recvDone() && rf12_crc == 0 && rf12_hdr == (RF12_HDR_DST | RF12_HDR_CTL | NODEID))
      return 1;
     set_sleep_mode(SLEEP_MODE_IDLE);
     sleep_mode();
  }
  return 0;
}
      
static void transmissionRS()
{
  activityLed(1);
  Serial.println(' ');
//  Serial.print("LIGHT ");
//  Serial.println(measure.light);
//  Serial.print("HUMI ");
//  Serial.println(measure.humi);
//  Serial.print("TEMP ");
//  Serial.println(measure.temp);
//  Serial.print("PRES ");
//  Serial.println(measure.pressure);
//  Serial.print("NTC ");
//  Serial.println(measure.ntc);
//  Serial.print("LOBAT " );
//  Serial.println(measure.lobat, DEC);
//  Serial.print("BATVOL ");
//  Serial.println(measure.battvol);
//  Serial.print("SOLVOL ");
//  Serial.println(measure.solvol);
//  Serial.print("MBSOL ");
//  Serial.println(!pinState(MOSFET_SOL), DEC);
  activityLed(0);
}

static void activityLed (byte on) {
  pinMode(ACT_LED, OUTPUT);
  digitalWrite(ACT_LED, on);
  delay(150);
}

static void mosfet(byte fet, byte on) {
  pinMode(fet, OUTPUT);
  digitalWrite(fet, !on);
  delay(100);
}

/*
static byte solar_v1(int ldr)
{
  if (ldr > LDR_TR) {
    // enable
    mosfet(MOSFET_SOL, 1);
    measure.solar = 1;
  }
  else
  {
    // disable
    mosfet(MOSFET_SOL, 0);
    measure.solar = 0;
  }
  return measure.solar;
}
*/
static byte pinState(int pin)
{
  return digitalRead(pin) ? 1 : 0;
}
