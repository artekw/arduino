#include "Emon.h"
#include <VirtualWire.h>

EnergyMonitor emon;
char str[150];

//----------------------------------------------------------------------------
// Ustawienia uzytkownika
//----------------------------------------------------------------------------

int RFPin = 2;
int PWRLEDPin = 3;
int RFLEDPin = 4;
int DELAY = 5; // sekundy

//----------------------------------------------------------------------------
// Ustawienia
//----------------------------------------------------------------------------
void setup()
{
  // emon
  emon.setPins(A0,A1); //V, I
  //emon.calibration(1,1,1); // V, I, F
  emon.calibration(1.242666667, 0.196645991, -0.3); // V, I, F
  
  // nadajnik RF
  vw_set_ptt_inverted(true);
  vw_setup(2000);
  vw_set_tx_pin(RFPin);
  
  // diody
  pinMode(RFLEDPin, OUTPUT);
  pinMode(PWRLEDPin, OUTPUT);
  
  // debug
//  Serial.begin(9600);
}

//----------------------------------------------------------------------------
// Program główny
//----------------------------------------------------------------------------
void loop()
{
  digitalWrite(PWRLEDPin, HIGH);
  emon.calc(20,2000);              //Energy Monitor calc function 
  
  // debug
/*
  Serial.print(emon.realPower);
  Serial.print(' ');
  Serial.print(emon.apparentPower);
  Serial.print(' ');
  Serial.print(emon.powerFactor);
  Serial.print(' ');
  Serial.print(emon.Vrms);
  Serial.print(' ');
  Serial.println(emon.Irms);
*/
  createJSON();
  transmissionRF();
  
  memset(str, 0, sizeof(str)); // czyscimy tablice
  delay (DELAY*1000);
}
//----------------------------------------------------------------------------

void createJSON()
{  
//  strcpy(msg, str); 
  srtJSON(str);
  addJSON(str,"rP",  emon.realPower);
  // addJSON(str,"aP",  emon.apparentPower);
  addJSON(str,"pF",  emon.powerFactor);
  addJSON(str,"V",  emon.Vrms); 
  addJSON(str,"I",  emon.Irms);
  endJSON(str);
}

void transmissionRF()
{
  digitalWrite(RFLEDPin, true);
  vw_send((uint8_t *)str, strlen(str));
  vw_wait_tx();
  digitalWrite(RFLEDPin, false);
}
