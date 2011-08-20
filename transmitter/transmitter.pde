/*
Weather Station Transmitter v1.0-dev
Written by Artur Wronowski <artur.wronowski@digi-led.pl>
Works with optiboot too.
*/

#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SHT21.h>
#include <BMP085.h>
#include <RF12.h>
#include <Ports.h>
#include <weather_data.h>
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
#define MEASURE_PERIOD    100 //300
#define RETRY_ACK         3  //? // how many times try get ack
#define REPORT_EVERY      20 //20  
#define ACK_TIME          10 //? // time for recive ack packet (in milisec)
#define RETRY_PERIOD      10 //?
#define LDR_TR            150 // LDR treshold, over activate solar
static byte NODEID = 2;       // ID this node

//#define ObwAnem 0.25434 // meters

// Used devices and buses
#define ONEWIRE           0 // use 1wire bus
#define I2C               1 // use i2c bus
#define SHT21_TEMP        1 // read temperature from SHT21 (only if I2C bus is used) not implemented yet
#define DEBUG             0 // debug mode - serial
#define LED_ON            0 // use act led - transmission
#define SOLAR             0 // use solar to power device and charge batteries
#define SOLAR_V1          0

#define RADIO_SYNC_MODE   2

/************************************************************/

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

enum { MEASURE, REPORT, TASKS };

static word schedbuf[TASKS];
Scheduler scheduler (schedbuf, TASKS);

int seq = 0;
static byte reportCount;

#if ONEWIRE
    OneWire oneWire(ONEWIRE_DATA);
    DallasTemperature sensors(&oneWire);
#endif

void setup()
{
    rf12_initialize(NODEID, RF12_433MHZ, 212);
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

