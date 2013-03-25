#include <JeeLib.h>
#include <avr/sleep.h>
#include <Wire.h>

float wolty;
float output;
float ofset = 0;
byte c, d, e;
byte x = 0x8C; // wzmocnienie: 0x8C *1, 0x8D *2, 0x8E *4, 0x8F *8

float avg, tmp, res, vol = 0.0;

typedef struct {
    long lp;
    int temp;
} Payload;
Payload pomiar;

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

MilliTimer sendTimer;
void setup () {
  Serial.begin(9600);
  rf12_initialize(11, RF12_433MHZ, 210);
  pinMode(9, OUTPUT);
  pomiar.lp=0;
 //   rf12_sleep(RF12_SLEEP);

  Wire.begin();        // join i2c bus (address optional for master)
  Wire.beginTransmission(0x4F); // transmit to device #48
  Wire.write(x);              // sends one byte
  Wire.endTransmission();    // stop transmitting
}

int ads1110() {
  Wire.requestFrom(0x4F,3);
  while(Wire.available())
    {
      c = Wire.read(); // starszy bajt * 256
      d = Wire.read(); // mlodszy bajt
      e = Wire.read(); // konfiguracja przetwornika
    }
  output = c * 256;
  output = output + d + ofset;
  wolty = output * 2.048 ;
  wolty = wolty / 32767.0000000;
  res = wolty / 0.00091;
  for (byte i = 0; i < 3; i++) {
    tmp = GetPlatinumRTD(res, 1000);
    avg = avg + tmp;
  }
  return (avg/3)*100;
  //delay(2000);
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

void loop () {
  pomiar.temp = ads1110();
  avg = 0;
  digitalWrite(9, HIGH);
 // rf12_sleep(RF12_SLEEP);
    rf12_recvDone();
    if (rf12_canSend()) {
      rf12_sendStart(0, &pomiar, sizeof pomiar,1);
      pomiar.lp += 1;
      digitalWrite(9, LOW);
    }
 // rf12_sleep(RF12_WAKEUP);

Sleepy::loseSomeTime(5000);

}
/*
long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts

}*/