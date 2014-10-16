//pt1000node

#include <Wire.h>
#include <JeeLib.h>
//#include <avr/sleep.h>

float wolty, avg, tmp, res, vol = 0.0;
float output;
float ofset = 50.0;
byte c, d, e;
byte x = 0x8C; // wzmocnienie: 0x8C *1, 0x8D *2, 0x8E *4, 0x8F *8

typedef struct {
  int light;
  int humi;
  int temp;
  int pressure;
  byte lobat      :1;
  int battvol;
} Payload;
Payload pomiar;

//ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup() {
  //Serial.begin(9600);
  Wire.begin();        // join i2c bus (address optional for master)
  Wire.beginTransmission(0x4F); // transmit to device #48
  Wire.write(x);              // sends one byte
  Wire.endTransmission();    // stop transmitting
  rf12_initialize(12, RF12_433MHZ, 210);
  pinMode(9, OUTPUT);
}

float ads1110(float offset) {
  Wire.requestFrom(0x4F,3);
  while(Wire.available())
  {
    c = Wire.read(); // starszy bajt * 256
    d = Wire.read(); // mlodszy bajt
    e = Wire.read(); // konfiguracja przetwornika
  }
  output = c * 256;
  output = output + d + offset;
  return output;
}

float GetPlatinumRTD(float R,float R0) {
  float A=3.9083E-3;
  float B=-5.775E-7;
  float T;

  R=R/R0;

  //T = (0.0-A + sqrt((A*A) - 4.0 * B * (1.0 - R))) / 2.0 * B;
  T=0.0-A;
  T+=sqrt((A*A) - 4.0 * B * (1.0 - R));
  T/=(2.0 * B);

  if(T>0&&T<200) {
    return T;
  }
  else {
    //T=  (0.0-A - sqrt((A*A) - 4.0 * B * (1.0 - R))) / 2.0 * B;
    T=0.0-A;
    T-=sqrt((A*A) - 4.0 * B * (1.0 - R));
    T/=(2.0 * B);
    return T;
  }
}

void loop() {
  output = ads1110(ofset);

  wolty = output * 2.048 ;
  wolty = wolty / 32767.0000000;
  res = wolty / 0.00106; // prąd w A 1.057142857
  for (byte i = 0; i < 10; i++) {
    //tmp = GetPlatinumRTD(res, 1000);
    tmp = (9E-10 * res * res * res) + (7E-06 * res *res) + (0.239 * res) - 247.3; // z moich obliczen
    avg = avg + tmp;
  }
  //Serial.println(avg/10);
  //delay(2);
  pomiar.temp = (avg/10) * 100;
  avg = 0;

  digitalWrite(9, HIGH);
  rf12_recvDone();
  if (rf12_canSend()) {
    rf12_sendStart(0, &pomiar, sizeof pomiar,1);
    digitalWrite(9, LOW);
  }

Sleepy::loseSomeTime(5000);
}