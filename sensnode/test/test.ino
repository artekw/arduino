#include <JeeLib.h>
// avr sleep instructions
#include <avr/sleep.h>

// structure of data
typedef struct {
	int nodeid;
	int light;
	float humi;
	float temp;
	float pressure;
	byte lobat		:1;
	int battvol;
	byte fet		:1;
} Payload;
Payload measure;

static byte NODEID = 9;       // ID this node
static byte NODEGROUP = 212;

unsigned int ADCValue;
double Voltage;
double Vcc;

#define BatVol     0
#define Solar      8

#define VCC_OK 350
#define VCC_LOW 340
#define VCC_CRI 330

#define VCC_SLEEP_MINS(x) ((x) > VCC_LOW ? 1 : (x) >= VCC_

#define SEND_MODE   2

#define rf12_sleep(x)

Port batvol (BatVol+1);

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup() {                
  Serial.begin(9600); 
  rf12_initialize(NODEID, RF12_433MHZ, NODEGROUP);
  rf12_control(0xC040); // 2.2v low
  rf12_sleep(RF12_SLEEP);
  digitalWrite(Solar, HIGH);
}

long readVcc() {

  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1125300L / result; // Back-calculate AVcc in mV
  return result;
}

static void doReport()
{
  rf12_sleep(RF12_WAKEUP);
  while (!rf12_canSend())
    rf12_recvDone();
  rf12_sendStart(0, &measure, sizeof measure);
  rf12_sendWait(SEND_MODE);
  rf12_sleep(RF12_SLEEP);
}

static void doMeasure() {
  measure.nodeid = NODEID;
  measure.lobat = rf12_lowbat();
  measure.battvol = batRead();
}

float batRead() {
  Vcc = readVcc()/10.0;
  ADCValue = batvol.anaRead() * 2.0;
  Voltage = (ADCValue / 1023.0) * Vcc;
  return Voltage;
}

void loop() {
  measure.nodeid = NODEID;
  measure.lobat = rf12_lowbat();
  int vcc = batRead();
  if (vcc <= VCC_OK) {
    measure.battvol = batRead();
  }
//  doMeasure();
  doReport();
  byte mins = VCC_SLEEP_MINS(vcc);
  while (mins-- < 0)
    Sleepy::loseSomeTime(60000);
}
