// ADS1110 
#include <Wire.h> 
float wolty; 
float output; 
float alfa = 1;
float ofset = 0; 
byte c, d, e; 
byte x = 0x8C; // wzmocnienie: 0x8C *1, 0x8D *2, 0x8E *4, 0x8F *8 
// 10001100 ciągły pomiar  0x8C 16 bit , 15pps, max 2,048V 
// 10011100 pojedynczy pomiar 0x9C wymaga dodatkowego inicjowania pojedynczego pomiaru przez zapis tego bajtu w pętli loop 

void setup() 
{ 
  Wire.begin();        // join i2c bus (address optional for master) 
  Serial.begin(9600);  // start serial for output
  Serial.println("ADS1110");
  Wire.beginTransmission(0x4F); // transmit to device #48 
  Wire.write(x);              // sends one byte 
  Wire.endTransmission();    // stop transmitting 
} 
void loop() 
{ 
  
  // pobudzenie pomiaru 
  /*
  Wire.beginTransmission(0x4F); // transmit to device #48 
  Wire.write(x);              // sends one byte 
  Wire.endTransmission();    // stop transmitting 
  /// koniec pomiaru 
  delay(200); //odczekanie na pomiar 
  */
  Wire.requestFrom(0x4F,3);    // request 3 bytes from slave device #2 
  while(Wire.available())    // slave may send less than requested 
  { 
    c = Wire.read(); // starszy bajt * 256
    d = Wire.read(); // mlodszy bajt
    e = Wire.read(); // ??
  } 
  output = c * 256; 
  output = output + d + ofset; 
  Serial.println(output);
  wolty = output * 2.048 ;
  wolty = wolty / 32767.0; 
  // wolty = wolty / alfa; 
  Serial.println("---------------"); 
  Serial.println(wolty, DEC); 
  Serial.println("---------------"); 
  delay(1000);
}
/*
09/01/2013 22:05:10 tytan: najlepiej ustaw na pomiar koło 1V i odczytuj te trzy bajty
09/01/2013 22:05:27 tytan: pierwszy +256 + drugi i patrz co to jest
09/01/2013 22:06:01 tytan: pierwszy * 256 + drugi a trzeci do kosza
09/01/2013 22:17:05 tytan: jako X wstawiaj 8C
09/01/2013 22:17:24 tytan: czyli mnożnik x1 , 16, it i pomiar ciąły 15 razy na 1/s
*/