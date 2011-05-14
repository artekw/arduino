#include <Wire.h>

int var;

void setup() {
  Wire.begin();
  Serial.begin(9600);
}


void loop() {
Wire.beginTransmission(0x48);   // transmit to device #48  PCF8591
Wire.send(0x01);                // selection of alaog input 2  
Wire.endTransmission();        // stop transmitting

Wire.requestFrom(0x48, 1);    // request 1 byte
while(Wire.available())          // slave may send less than requested
{    
  var = Wire.receive();          // receive a byte as character
} Wire.requestFrom(0x48, 1);    // request 1 byte
while(Wire.available())          // slave may send less than requested
{    
  var = Wire.receive();  // receive a byte as character
  Serial.println(var%2);
  delay(500);
}
}
