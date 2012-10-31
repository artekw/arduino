#include <JeeLib.h>
// avr sleep instructions
#include <avr/sleep.h>

unsigned int ADCValue;
double Voltage;
double Vcc;

#define BatVolPin     1
#define SolVolPin     2

Port batvol (BatVolPin+1); // Analog 2

void setup() {
	Serial.begin(9600);
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

void loop() {
	Vcc = readVcc()/1000.0;
	ADCValue = batvol.anaRead() * 2.0;
	Voltage = (ADCValue / 1023.0) * Vcc;
	Serial.print(Vcc);
	Serial.print(" ");
	Serial.print(ADCValue);
	Serial.print(" ");
	Serial.println(Voltage);
	delay(1000);

}