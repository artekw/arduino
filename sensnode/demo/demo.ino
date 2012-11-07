#include <JeeLib.h>
// avr sleep instructions
#include <avr/sleep.h>

/*************************************************************/

// Input/Output definition
// Analog
#define LDR               2
#define BAT_VOL           3

// Digital
#define MOSFET            7
#define ONEWIRE_DATA      8
#define ACT_LED           9

// Settings
#define MEASURE_PERIOD    300 // how often to measure, in tenths of seconds
#define RETRY_PERIOD      10 // how soon to retry if ACK didn't come in
#define RETRY_ACK         5 // maximum number of times to retry
#define ACK_TIME          10 // number of milliseconds to wait for an ack
#define REPORT_EVERY      6 // report every N measurement cycles
#define SMOOTH            3 // smoothing factor used for running averages
#define RADIO_SYNC_MODE   2

/************************************************************/

Port batvol (4); // Analog 3


void setup()
{
  Serial.begin(9600);
  pinMode(ACT_LED, OUTPUT);
  pinMode(MOSFET, OUTPUT);
  digitalWrite(ACT_LED, HIGH);
//  digitalWrite(MOSFET, HIGH);
}
void loop()
{
  
  Serial.println(map(batvol.anaRead(), 0, 1023, 0, 660));
  delay(1000);
  aaa();
}

void aaa() {
  digitalWrite(MOSFET, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(MOSFET, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
}

