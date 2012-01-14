#include <JeeLib.h>
#include <OneWire.h>
#include <DallasTemperature.h>

typedef struct {
	int lp;
	float temperatura;
} Payload;
Payload pomiar;

MilliTimer sendTimer;

OneWire oneWire(8);
DallasTemperature sensors(&oneWire);


void setup () {
    rf12_initialize(1, RF12_433MHZ, 212);
    pinMode(9, OUTPUT);
    sensors.begin();
    pomiar.lp=0;
}

void loop () {
    sensors.requestTemperatures();
    pomiar.temperatura = sensors.getTempCByIndex(0);
    digitalWrite(9, LOW);

    rf12_recvDone();
    if (rf12_canSend() && sendTimer.poll(5000)) {
      rf12_sendStart(0, &pomiar, sizeof pomiar);
      pomiar.lp += 1;
      digitalWrite(9, HIGH);
    }
}
