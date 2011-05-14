#include "LedControl.h"
#include <Wire.h>
#include <ds1307.h>
//#include "RTClib.h"
#include <Bounce.h>

#define BUTTON 2

//RTC_DS1307 RTC;

LedControl lc=LedControl(12,11,10,2);
Bounce bouncer = Bounce( BUTTON,5 ); 

void setup() {
  pinMode(BUTTON, INPUT);
  
  int devices=lc.getDeviceCount();
  
  for(int address=0;address<devices;address++) {
    lc.shutdown(address,false);
    lc.setIntensity(address,1);
    lc.clearDisplay(address);
  }
    Serial.begin(9600);
//    Wire.begin();
//    RTC.begin();

/*
  RTC.stop();
  RTC.set(DS1307_SEC,0);
  RTC.set(DS1307_MIN,6);
  RTC.set(DS1307_HR,16);
//  RTC.set(DS1307_DOW,4);
  RTC.set(DS1307_DATE,13);
  RTC.set(DS1307_MTH,2);
  RTC.set(DS1307_YR,10);
  RTC.start();
*/
/*
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //RTC.adjust(DateTime(__DATE__, __TIME__));
  }
*/
}
int rtc[7];

byte cyfry[][4] = {
  {B01111111,B01000001,B01111111,B00000000}, //0
  {B00100001,B01111111,B00000001,B00000000}, //1
  {B01001111,B01001001,B01111001,B00000000}, //2
  {B01001001,B01001001,B01111111,B00000000}, //3
  {B01111000,B00001000,B01111111,B00000000}, //4
  {B01111001,B01001001,B01001111,B00000000}, //5
  {B01111111,B01001001,B01001111,B00000000}, //6
  {B01000000,B01000000,B01111111,B00000000}, //7
  {B01111111,B01001001,B01111111,B00000000}, //8
  {B01111001,B01001001,B01111111,B00000000}  //9
};

void czas(int hour, int min, int sec) {
 int j,k = 0;
 
 int h1= hour / 10;
 int h2= hour % 10;
 int m1= min / 10;
 int m2= min % 10;
 int s= sec % 2;

  for (j=0; j<4; j++) {
      lc.setRow(0,0+j,cyfry[h1][j]);
      }
  for (k=0; k<4; k++) {
        lc.setRow(0,4+k, cyfry[h2][k]);
      }
  for (j=0; j<4; j++) {
      lc.setRow(1,0+j,cyfry[m1][j]);
      }
  for (k=0; k<4; k++) {
        lc.setRow(1,4+k, cyfry[m2][k]);
      }
  // very ugly
  if (s == 0) 
  {
    lc.setLed(1,7, 7, true);
  }
  else 
  {
    lc. setLed(1,7, 7, false);
  }
}

void data (int day, int month) {
  
 int j,k = 0;
 int d1= day / 10;
 int d2= day % 10;
 int m1= month / 10;
 int m2= month % 10;

 for (j=0; j<4; j++) {
      lc.setRow(0,0+j,cyfry[d1][j]);
      }
 for (k=0; k<4; k++) {
        lc.setRow(0,4+k, cyfry[d2][k]);
      }
 for (j=0; j<4; j++) {
      lc.setRow(1,0+j,cyfry[m1][j]);
      }
 for (k=0; k<4; k++) {
        lc.setRow(1,4+k, cyfry[m2][k]);
      }  
 
}
void loop() { 

  RTC.get(rtc,true);
//  DateTime now = RTC.now();
  bouncer.update();
  
  int value = bouncer.read();
  
  // data po przyciskiem
  if (value == LOW) 
  {
     czas(rtc[2], rtc[1], rtc[0]);
  }
  else
  {
     data(rtc[4], rtc[5]);
  }

  if (rtc[2] >= 22 || rtc[2] >=6)
  {
    lc.setIntensity(0,0);
    lc.setIntensity(1,0);
  }

}
