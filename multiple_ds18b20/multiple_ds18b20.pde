#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 7
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup(void)
{
Serial.begin(9600);
sensors.begin();
}
void loop(void)
{
sensors.requestTemperatures();
Serial.println("");
delay(100);
Serial.print("Czujnik-1: ");
Serial.println(sensors.getTempCByIndex(0));

Serial.print("Czujnik-2: ");
Serial.println(sensors.getTempCByIndex(1));

Serial.print("Czujnik-3: ");
Serial.println(sensors.getTempCByIndex(2));
}
