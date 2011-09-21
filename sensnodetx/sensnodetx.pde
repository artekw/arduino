/*
SensnodeTX v1.0-dev
Written by Artur Wronowski <artur.wronowski@digi-led.pl>
Works with optiboot too.
*/

// libs for I2C and 1Wire
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SHT21.h>
#include <BMP085.h>
// libs for RFM12B from <jeelabs.org>
#include <Ports.h>
#include <RF12.h>
// custom data structure
#include <weather_data.h>
// avr sleep instructions
#include <avr/sleep.h>

/*************************************************************/
// Input/Output definition
// Analog
#define LDRPin            0
#define WindPin           2
#define BatteryVolPin     3
#define SolarVolPin       1 // not used in v1

// Digital
#define ACT_LED           3
#define ONEWIRE_DATA      4
#define MOSFET_SOL        5
#define MOSFET_BAT        6 // not used in v1

// Settings
#define MEASURE_PERIOD    5 //300
#define RETRY_ACK         3  //? // how many times try get ack
#define REPORT_EVERY      20 //20  
#define ACK_TIME          10 //? // time for recive ack packet (in milisec)
#define RETRY_PERIOD      10 //?
#define LDR_TR            150 // LDR treshold, over activate solar
static byte NODEID = 2;       // ID this node
static byte NODEGROUP = 212;

//#define ObwAnem 0.25434 // meters

// Used devices and buses
#define ONEWIRE           0 // use 1wire bus
#define I2C               1 // use i2c bus
#define SHT21_TEMP        1 // read temperature from SHT21 (only if I2C bus is used) not implemented yet
#define DEBUG             1 // debug mode - serial
#define LED_ON            0 // use act led - transmission
#define SOLAR             0 // use solar to power device and charge batteries
#define SOLAR_V1          0

#define RADIO_SYNC_MODE   2

/************************************************************/

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

enum { MEASURE, REPORT, TASKS };

static word schedbuf[TASKS];
Scheduler scheduler (schedbuf, TASKS);

// int seq = 0;
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
          doReport();
        }
        else
        {
          doReport();
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
    rf12_sendStart(RF12_HDR_ACK, &pomiar, sizeof pomiar);
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
  rf12_sendStart(0, &pomiar, sizeof pomiar);
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
  Serial.print("LIGHT ");
  Serial.println(pomiar.light);
  Serial.print("HUMI ");
  Serial.println(pomiar.humi);
  Serial.print("TEMP ");
  Serial.println(pomiar.temp);
  Serial.print("PRES ");
  Serial.println(pomiar.pressure);
//  Serial.print("SPD ");
//  Serial.println(pomiar.wind);
//  Serial.print("LOBAT " );
//  Serial.println(pomiar.lobat, DEC);
  Serial.print("BATVOL ");
  Serial.println(pomiar.battvol);
  Serial.print("SOLVOL ");
  Serial.println(pomiar.solvol);
  Serial.print("MSOL ");
  Serial.println(!pinState(MOSFET_SOL), DEC);
  Serial.print("MBAT ");
  Serial.println(!pinState(MOSFET_BAT), DEC);
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

static byte solar(int ldr)
{
  byte tmp;
  if (ldr > LDR_TR) {
    mosfet(MOSFET_SOL, 1);
    mosfet(MOSFET_BAT, 1);
    tmp = 1;
  }
  else
  {
    mosfet(MOSFET_SOL, 0);
    mosfet(MOSFET_BAT, 0);
    tmp = 0;
  }
  return (pomiar.solar = tmp, pomiar.bat = tmp);
}

static byte solar_v1(int ldr)
{
  if (ldr > LDR_TR) {
    // enable
    mosfet(MOSFET_SOL, 1);
    pomiar.solar = 1;
  }
  else
  {
    // disable
    mosfet(MOSFET_SOL, 0);
    pomiar.solar = 0;
  }
  return pomiar.solar;
}

static byte pinState(int pin)
{
  return digitalRead(pin) ? 1 : 0;
}
