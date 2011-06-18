/*
Weather Station Transmitter v1.0-dev
Written by Artur Wronowski <artur.wronowski@digi-led.pl>
*/

#include <Wire.h>
#include <SHT21.h>
#include <BMP085.h>
#include <RF12.h>
#include <Ports.h>
#include <weather_data.h>
#include <avr/sleep.h>

/*************************************************************/
// Input/Output definition
// Analog
#define LDRPin            A0
#define BatteryPin        A1
#define SolarPin          A2
#define WindPin           A3 //?

// Digital
#define ACT_LED           3
#define ONEWIRE_DATA      4
#define MOSFET_SOL        7
#define MOSFET_BAT        8

// Settings
#define MEASURE_PERIOD    300 //60
#define RETRY_ACK         5  //?
#define REPORT_EVERY      15 //10 
#define ACK_TIME          10 //?
#define RETRY_PERIOD      10 //?
#define SOLAR             100

//#define ObwAnem 0.25434 // metry

#define ONEWIRE           0
#define I2C               1
#define DEBUG             0
#define LED_ON            0

#define NODEID            1
#define RADIO_SYNC_MODE   2

/*************************************************************/

//EMPTY_INTERRUPT(WDT_vect);

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

enum { MEASURE, REPORT, TASKS };

static word schedbuf[TASKS];
Scheduler scheduler (schedbuf, TASKS);

int seq = 0;
static byte reportCount;

void setup()
{
    rf12_initialize(NODEID, RF12_433MHZ, 212);
    rf12_sleep(0);
#if I2C
      Wire.begin();
      BMP085.getCalData();
#endif
#if DEBUG
      Serial.begin(9600);
      Serial.println("DEBUG MODE");
#endif

reportCount = REPORT_EVERY;
scheduler.timer(MEASURE, 0);
}

void loop()
{
  switch (scheduler.pollWaiting()) {

    case MEASURE:
        scheduler.timer(MEASURE, MEASURE_PERIOD);
        doMeasure();

        if (reportCount++ >= REPORT_EVERY) {
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

