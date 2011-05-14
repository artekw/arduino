
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 7

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

double val = 0;

void setup(void)
{
Serial.begin(115200);

sensors.begin();
}

void loop(void)
{
sensors.requestTemperatures();
Serial.println("");
// usredniamy
val = 0;
for(int i = 0; i < 10; i++) {
val += sensors.getTempCByIndex(0);
delay(200);
}
Serial.print(val/10);
}

