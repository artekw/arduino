#include <Wire.h>

#define UNO (4+2)
#define DUE (1+2+64+16+8)
#define TRE (1+2+64+4+8)
#define QUATTRO (32+64+2+4)
#define CINQUE  (1+32+64+4+8)
#define SEI  (1+32+64+4+8+16)
#define SETTE  (1+32+16)
#define OTTO  (127)
#define NOVE (1+2+64+32+16+8)
#define ZERO (1+2+4+8+16+32)
#define PUNTO (128)

byte display_add1 = 0x72 >> 1;  // check datasheet for the address of the device
byte display_add2 = 0x70 >> 1;  // left shit 1 bit the device addr to comply with Wire Lib
                                // the lsb of the addr is the read/write bit and is added 
                                // automagically by the Wire lib this is why we nedd to take out the lsb from addr. 
                                // If the write addr is 0x70 than the read addr is 0x71, the lsb is changed from write to read.
                                // Check the code for the Wire lib to understand why you need to left shit 1 bit
                                // 0x70 and 0x72 was the original hardware addr of the chips as set in the board.

int ledPin = 13;                // LED connected to digital pin 13

void setup()                    // run once, when the sketch starts
{
  pinMode(ledPin, OUTPUT);      // sets the digital pin as output
  Wire.begin(); // join i2c bus (address optional for master)
}

void loop()                     // run over and over again
{
  digitalWrite(ledPin, HIGH);   // sets the LED on
  test_num_display();
  testi2c();
  digitalWrite(ledPin, LOW);    // sets the LED off
  delay(50);
}


void test_num_display()
{
  byte digit_num = 0 ;

  clear_display();
  for(digit_num=1;digit_num<5;digit_num++) {    
    display_num(display_add1,( byte)digit_num,digit_num);
    delay(50);
  }
  for(digit_num=1;digit_num<3;digit_num++) {    
    display_num(display_add2,( byte)digit_num,digit_num+4);
    delay(50);
  }
  delay(2000);
}


