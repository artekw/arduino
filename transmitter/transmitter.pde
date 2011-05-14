/*
Nadajnik stacji metrologicznej v0.01
Written by Artur Wronowski <artur.wronowski@digi-led.pl>

Wilgotność i temperatura - Sensirion SHT21 / I2C
Ciśnienie - Bosch BMP085 / I2C
Jasność - Fotorezystor / analog
Transmisja - RF 433MHz / UART
Pomiar stanu baterii - analog
Pomiar prędkości wiatru - analogowy anemometr
Tryb oszczedzania energii - BRAK
"Podbijacz napięcia" - BRAK
*/

#include <VirtualWire.h>
#include <Wire.h>
#include <SHT21.h>
#include <BMP085.h>
//#include <avr/sleep.h>

/*************************************************************/
// Definicja wyjść
#define LDRPin A0
#define BatteryPin A1
#define WindPin A2
#define RFLEDPin 13
#define RFPin 11

// Ustawienia
#define DELAY 5 // sek
#define ResLDR 10.0 //rezystor(10k) dla fotorezystora
#define BMP85Korekta 16 //16kPh
#define OAnemometr 0.25434 // metry
#define NapKrytyczne 1.5 // 1.5 Volta
#define RFBps 2000 //Bps

#define DEBUG true
#define NOI2C true
/*************************************************************/

// Zmienne
char str[200];
float tmp;
int LDRVal = 0;
int BatteryVal = 0;

void setup()
{
    vw_set_ptt_inverted(true);
    vw_setup(RFBps);	 // Bits per sec
    vw_set_tx_pin(RFPin);   // Pin do wysyłania
    pinMode(RFLEDPin, OUTPUT);
    if (!NOI2C) {
      Wire.begin();
      BMP085.getCalData();
    }
    if (DEBUG) {
      Serial.begin(9600);
      Serial.println("DEBUG MODE");
    }

}

void loop()
{
  if (!NOI2C) {
    SHT21.readSensor();
    BMP085.readSensor();
  }
  createJSON();
  
  if (DEBUG) {
    transmissionRS();
  }
  else
  {
    transmissionRF();
  }
  
  memset(str, 0, sizeof(str)); // czyscimy tablice
  delay (DELAY*1000);
}

float BatteryVoltage()
{
  // dzielnik napięcia 1/6 (10k + 50k lub 2k (?))
  BatteryVal = analogRead(BatteryPin);
  float volts = (BatteryVal * 5.0 / 0.833) / 1024.0;
  return volts;
}

float getSHT21(char opt)
{
  if (opt == 'h' )
  {
    tmp = SHT21.humi;
  }
  else if (opt == 't')
  {
    tmp = SHT21.temp;
  }
  
  return tmp;
}

float getBMP085(char opt)
{
  if (opt == 'p' )
  {
    tmp = (BMP085.press*10) + BMP85Korekta;
  }
  else if (opt == 't')
  {
    tmp = BMP085.temp;
  }
  
  return tmp;
}

float getLDR()
{
  LDRVal = analogRead(LDRPin);
  float vo = LDRVal * (5.0 / 1024.0);
  float lux = 500/(ResLDR*((5.0-vo)/vo));
  return lux;
}  

float getWind()
{
  //
}

void transmissionRF()
{
  digitalWrite(RFLEDPin, true);
  vw_send((uint8_t *)str, strlen(str));
  vw_wait_tx();
  digitalWrite(RFLEDPin, false);
}

void transmissionRS()
{
  Serial.println(str);
}

void createJSON()
{  
  srtJSON(str);
  addJSON(str,"LDR",  getLDR());
  addJSON(str,"VOL",  BatteryVoltage());
  if (!NOI2C) {
    addJSON(str,"RH",  getSHT21('h'));
    addJSON(str,"T",  getSHT21('t'));
    addJSON(str,"PR",  getBMP085('p'));
  }
  endJSON(str);
}
