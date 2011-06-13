/*
Nadajnik stacji metrologicznej v0.1
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
// Definicja wejść/wyjść
// Analaogowe
#define LDRPin 14
#define BatteryPin 15
//#define WindPin 16
// Cyfrowe
#define ACT_LED 3
//#define ONEWIRE_DATA 3
#define MOSFET_GATE 7

// Ustawienia
#define SLEEP_TIME 30000
//#define ResLDR 10.0 //rezystor(10k) dla fotorezystora
//#define ObwAnem 0.25434 // metry

//#define ONEWIRE false
#define I2C true
#define DEBUG false

/*************************************************************/

//EMPTY_INTERRUPT(WDT_vect);
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup()
{
    rf12_initialize(2, RF12_433MHZ, 212);
    rf12_sleep(0);
#if I2C
      Wire.begin();
      BMP085.getCalData();
#endif
#if DEBUG
      Serial.begin(9600);
      Serial.println("DEBUG MODE");
#endif
}

void loop()
{
#if I2C
    SHT21.readSensor();
    BMP085.readSensor();
#endif
#if I2C
    getSHT21('h');
    getSHT21('t');
    getBMP085();
#endif
  pomiar.lobat = rf12_lowbat();  
  getLDR();
//  battVol();
//  solarOn(getLDR());

  if (DEBUG) {
    transmissionRS();
  }
  else
  {
    transmissionRF();
  }
  // zzz...
  for (byte i = 0; i < 1; i++ ) {
    Sleepy::loseSomeTime(SLEEP_TIME);
  }

}
