void testi2c()
{
  clear_display();
  byte digit=0;
  byte i;
  for (i=1;i<5;i++)
  {
    do {
      Wire.beginTransmission(display_add1); // transmit to device  
      Wire.write(i);
      digit = digit << 1;
      digit = digit + B1;
      Wire.write(digit);
      delay(50);
      Wire.endTransmission();     // stop transmitting
    } 
    while (digit < B11111111);
    digit = 0 ;
  }


  for (i=1;i<5;i++)
  {
    do {
      Wire.beginTransmission(display_add2); // transmit to device 
      Wire.write(i);
      digit = digit << 1;
      digit = digit + B1;
      Wire.write(digit);
      delay(50);
      Wire.endTransmission();     // stop transmitting
    } 
    while (digit < B11111111);
    digit = 0 ;
  }

  for (i=4;i>0;i--)
  {
    digit = B11111111;
    while(digit>B0) {
      Wire.beginTransmission(display_add2); // transmit to device 
      Wire.write(i);
      digit = digit >> 1;
      Wire.write(digit);
      delay(50);
      Wire.endTransmission();     // stop transmitting
    } 
  }

  for (i=4;i>0;i--)
  {
    digit = B11111111;
    while(digit>B0) {
      Wire.beginTransmission(display_add1); // transmit to device
      Wire.write(i);
      digit = digit >> 1;
      Wire.write(digit);
      delay(50);
      Wire.endTransmission();     // stop transmitting
    } 
  }

}
