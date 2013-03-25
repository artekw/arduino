#include <PCF8574.h>
#include <Wire.h>

PCF8574 expander;

void setup()
{
  expander.begin(0x38);
  expander.pinMode(0, OUTPUT);
}

void loop()
{
//  expander.digitalWrite(0, LOW);
//  delay(1000);
  expander.digitalWrite(0, HIGH);
  expander.pullUp(0);
  delay(1000);
}