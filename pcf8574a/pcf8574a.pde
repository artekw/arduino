/*
  Test program for PCF8574 I2C I/O expander
  - Blinks all ports low then high.
  by Ford
*/

#include <Wire.h>

#define expander 0x38  // Address with three address pins grounded.
				   // Note that the R/W bit is not part of this address.

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  Serial.println("Writing 0xAA.");
  expanderWrite(0xAA);
  Serial.print("Read: ");
  Serial.println(expanderRead(), BIN);
  delay(1000);
  Serial.println("Writing 0x00.");
  expanderWrite(0x00);
  Serial.print("Read: ");
  Serial.println(expanderRead(), BIN);
  delay(1000);
}


void expanderWrite(byte _data ) {
  Wire.beginTransmission(expander);
  Wire.send(_data);
  Wire.endTransmission();
}

byte expanderRead() {
  byte _data;
  Wire.requestFrom(expander, 1);
  if(Wire.available()) {
    _data = Wire.receive();
  }
  return _data;
}
 
