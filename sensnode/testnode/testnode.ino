#include <JeeLib.h>

#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BMP085.h>
#include <SHT2x.h>


// piny
#define LDR               2
#define BAT_VOL           3
#define MOSFET            7
#define ONEWIRE_DATA      8
#define ACT_LED           9

// radio
#define thisNodeID        6
#define network           210

// porty
Port p1 (1); // JeeLabs Port P1
Port p2 (2); // JeeLabs Port P2
Port ldr (3);  // Analog pin 2
Port batvol (4); // Analog pin 3

typedef struct {
	byte nodeid;
	int value;
	int battvol;
} Payload;
Payload testdata;

// do usypiania

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

MilliTimer sendTimer;

void setup()
{
	Serial.begin(9600);
	rf12_initialize(thisNodeID, RF12_433MHZ, network);
	testdata.value=0;
}

void testLED(byte on)
{
	pinMode(ACT_LED, OUTPUT);
	digitalWrite(ACT_LED, on);
	delay(150);
}

void testRF() {
	rf12_sleep(RF12_SLEEP);
		rf12_recvDone();
    	if (rf12_canSend() && sendTimer.poll(5000)) {
      		rf12_sendStart(0, &testdata, sizeof testdata,1);
			testdata.value += 1;

		}
	rf12_sleep(RF12_WAKEUP);
	Sleepy::loseSomeTime(2000);
}

void fettest() {
	pinMode(MOSFET, OUTPUT);
	digitalWrite(MOSFET, 1);
	delay(1000);
	digitalWrite(MOSFET, 0);
	delay(5000);
}

void loop() {
	//testLED(1);
	//testLED(0);
	//testRF();
	fettest();

}