
#include <Wire.h>
#include <SHT21.h>

int inc;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
}

void loop()
{
  SHT21.readSensor();
  if (Serial.available() > 0) {
    inc = Serial.read() % 2;
  }
  if (inc == 1)
  {
    readFromSensor();
  }
}
  
void readFromSensor() {
  Serial.print("Humidity(%RH): ");
  Serial.print(SHT21.humi, 1);
  Serial.print("     Temperature(C): ");
  Serial.println(SHT21.temp, 1);
}
