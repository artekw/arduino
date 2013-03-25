// http://arduino.cc/forum/index.php/topic,43605.0.html

#include <Wire.h>
float wolty;
float output;
float ofset = 0;
byte c, d, e;
byte x = 0x8C; // wzmocnienie: 0x8C *1, 0x8D *2, 0x8E *4, 0x8F *8

float avg, tmp, res, vol = 0.0;

void setup() {
	Serial.begin(9600);
	Serial.println("[pt1000]");
  Serial.println("[adc][nap][rez][temp]");
	Wire.begin();        // join i2c bus (address optional for master)
 	Wire.beginTransmission(0x4F); // transmit to device #48
	Wire.write(x);              // sends one byte
	Wire.endTransmission();    // stop transmitting
}

void loop() {
	Wire.requestFrom(0x4F,3);
	while(Wire.available())
  	{
    	c = Wire.read(); // starszy bajt * 256
    	d = Wire.read(); // mlodszy bajt
    	e = Wire.read(); // konfiguracja przetwornika
  	}
  	output = c * 256;
  	output = output + d + ofset;
  	Serial.print(output);
  	Serial.print(' ');
  	wolty = output * 2.048 ;
  	wolty = wolty / 32767.0000000;
	Serial.print(wolty, DEC);
	Serial.print(' ');
	res = wolty / 0.00091;
	Serial.print(res);
	Serial.print(' ');
	for (byte i = 0; i < 100; i++) {
		tmp = GetPlatinumRTD(res, 1000);
		avg = avg + tmp;
	}
	Serial.print(avg/100);
	Serial.print("*C");
	avg = 0;
	Serial.println();

	delay(2000);
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