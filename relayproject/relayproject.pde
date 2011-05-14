#include <EEPROM.h>
#include <WProgram.h>
#include <Wire.h>
#include <DS1307.h> 

byte relay, state = 0;
byte RTC_ENABLE = 0;
char cmd[5];
int minute1, minute2 = 0;
int hour1, hour2 = 0;
int relay1pin = 12;
int relay2pin = 11;

void setup() 
{                
  pinMode(relay1pin, OUTPUT);
  pinMode(relay2pin, OUTPUT);
  Serial.begin(9600);
  if (RTC_ENABLE == 1 )
  {
    RTC.stop();
    RTC.set(DS1307_MIN,00);
    RTC.set(DS1307_HR,00);
    RTC.start();
  }
}

void setRTC(int hour, int minute)
{
  // 
  RTC.stop();
  RTC.set(DS1307_MIN,minute);
  RTC.set(DS1307_HR,hour);
  RTC.start();
  Serial.print(RTC.get(DS1307_HR,true));
  Serial.print(":");
  Serial.print(RTC.get(DS1307_MIN,true));
  Serial.println();
}

void programRelay(byte nrelay, char job, byte hour, byte minute)
{
  // 
}

void loop() 
{
  // odczytanie stanów z pamięci
  if (EEPROM.read(1) == 1)
  {
     digitalWrite(relay1pin, HIGH);
  }
  if (EEPROM.read(2) == 1)
  {
     digitalWrite(relay2pin, HIGH);
  }
  // czekam na komendę
  while (Serial.available() >= 5)
    {
      char cmd[] = { Serial.read(), Serial.read(), Serial.read(), Serial.read(), Serial.read(), '\0' };
//      Serial.read();

switch (cmd[0]) 
{
  case 's':
    // decode
    relay = cmd[1] - 48;
    state = cmd[2] % 2;
    // zerowanie wszystkich
    if (relay == 0)
    {
      if (state == 0)
      {
        digitalWrite(relay1pin, LOW);
        digitalWrite(relay2pin, LOW);
        EEPROM.write(1,0);
        EEPROM.write(2,0);
      }
    }
    // przekaznik 1
    if (relay == 1)
    {
      if (state == 1) 
      {
        digitalWrite(relay1pin, HIGH);
        EEPROM.write(1,1);
      }
      else 
      { 
        digitalWrite(relay1pin, LOW);
        EEPROM.write(1,0);
      }
    }
    // przekaznik 2
    else if (relay == 2) 
    {
      if (state == 1) 
      {
        digitalWrite(relay2pin, HIGH);
        EEPROM.write(2,1);
      }
      else 
      { 
        digitalWrite(relay2pin, LOW);
        EEPROM.write(2,0);
      }  
    }
    break;
  case 'r':
    relay = cmd[1] - 48;
    Serial.println(EEPROM.read(relay), DEC);
  case 't':
    if (RTC_ENABLE == 1 )
  {
    minute1 = cmd[3] - 48;
    minute2 = cmd[4] - 48;
    hour1 = cmd[1] - 48;
    hour2 = cmd[2] - 48;
    setRTC((hour1 * 10) + hour2,(minute1 * 10 ) + minute2);
  }
    break;
  case 'd':
    if (RTC_ENABLE == 1 )
     {
    Serial.print(RTC.get(DS1307_HR,false));
    Serial.print(":");
    Serial.print(RTC.get(DS1307_MIN,false));
    Serial.println();
     }
    break;
  default:
    break;
}
}
}
