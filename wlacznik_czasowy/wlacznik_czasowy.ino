// aquarium controller

// pon-pt 5-21 (14.30-16.00)
// sob-nd 9-21 (14.30-16.00)

// https://github.com/adafruit/RTClib
// https://github.com/fouldsy/Arduino-RTC-Timer/blob/master/RTC_Light_Timer.pde

#include <Wire.h>
#include "RTClib.h"
#include <EEPROM.h>

RTC_DS1307 RTC;

byte startHour = 5;
byte startMin = 0;

byte endHour = 21;
byte endMin = 0;

byte wstartHour = 19;
byte wstartMin = 9;

byte feedTime = 0;
byte Start = 0;
byte PowerOn = EEPROM.read(0);

void setup () {
  Serial.begin(57600);
  Wire.begin();
  RTC.begin();

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  pinMode(2, OUTPUT); // LED
  pinMode(3, OUTPUT); // Relay

  teraz();

  if (EEPROM.read(0) == 1)
    relay(1);
}

byte relayState() {
  return EEPROM.read(0);
}
void teraz() {
    DateTime now = RTC.now();
    Serial.print("Teraz: ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print(' ');
    Serial.print(now.dayOfWeek(), DEC);
    Serial.println(' ');
}

void relay(byte on) {
  digitalWrite(2, on);
  digitalWrite(3, on);
}

byte harm(byte dow) {
  DateTime now = RTC.now();
  
  switch (dow) {
      case 1:
      if  (now.hour() == startHour && now.minute() == startMin) {
        EEPROM.write(0,1);
        relay(1);
      }
      if (now.hour() == endHour && now.minute() == endMin && PowerOn == 1) {
        EEPROM.write(0,0);
        relay(0);
      }
      break;
      case 2:
      if  (now.hour() == startHour && now.minute() == startMin) {
        EEPROM.write(0,1);
        relay(1);
      }
      if (now.hour() == endHour && now.minute() == endMin && PowerOn == 1) {
        EEPROM.write(0,0);
        relay(0);
      }
      break;
      case 3:
      if  (now.hour() == startHour && now.minute() == startMin) {
        EEPROM.write(0,1);
        relay(1);
      }
      if (now.hour() == endHour && now.minute() == endMin && PowerOn == 1) {
        EEPROM.write(0,0);
        relay(0);
      }
      break;
      case 4:
      if  (now.hour() == startHour && now.minute() == startMin) {
        EEPROM.write(0,1);
        relay(1);
      }
      if (now.hour() == endHour && now.minute() == endMin && PowerOn == 1) {
        EEPROM.write(0,0);
        relay(1);
      }
      break;
      case 5:
      if  (now.hour() == startHour && now.minute() == startMin) {
        EEPROM.write(0,1);
        relay(1);
      }
      if (now.hour() == endHour && now.minute() == endMin && PowerOn == 1) {
        EEPROM.write(0,0);
        relay(0);
      }
      break;
      case 6:
      if  (now.hour() == wstartHour && now.minute() == wstartMin) {
        EEPROM.write(0,1);
        relay(1);
      }
      if (now.hour() == endHour && now.minute() == endMin && PowerOn == 1) {
        EEPROM.write(0,0);
        relay(1);
      }
      break;
      case 7:
      if  (now.hour() == wstartHour && now.minute() == wstartMin) {
        EEPROM.write(0,1);
        relay(1);
      }
      if (now.hour() == endHour && now.minute() == endMin && PowerOn == 1) {
        EEPROM.write(0,0);
        relay(0);
      }
      break;
  }
}
     
byte checkStart() {
  DateTime now = RTC.now();
  if  (now.hour() == startHour && now.minute() == startMin) {
    Start = 1;
    EEPROM.write(0,1);
  } else {
    Start = 0;
//    EEPROM.write(0,0);
  }
  return Start;
}

void loop () {
  DateTime now = RTC.now();
  if (now.second() == 0) {
    harm(now.dayOfWeek());
  }
  delay(1000);
}
