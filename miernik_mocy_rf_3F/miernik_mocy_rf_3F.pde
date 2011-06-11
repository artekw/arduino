#include "Emon3f.h"
#include <VirtualWire.h>

EnergyMonitor emon;
char str[150];


//----------------------------------------------------------------------------
// Ustawienia uzytkownika
//----------------------------------------------------------------------------
#define RFPin 4
#define PWRLEDPin 3
#define RFLEDPin 2

#define DELAY 1
#define BPS 2000
#define DEBUG true

//----------------------------------------------------------------------------
// Ustawienia
//----------------------------------------------------------------------------
void setup()
{
  // emon
  emon.setPins(A0,A1,A2,A3); //V, I
  emon.calibration(0.705701078, 0.098253783, 1, 1, 1.3, 1); // V, I, F
  
  // nadajnik RF
  vw_set_ptt_inverted(true);
  vw_setup(1200);
  vw_set_tx_pin(RFPin);
  
  // diody
  pinMode(RFLEDPin, OUTPUT);
  pinMode(PWRLEDPin, OUTPUT);
  
  // debug
  if (DEBUG) {
    Serial.begin(9600);
    Serial.println("DEBUG");
  }
}

//----------------------------------------------------------------------------
// Program główny
//----------------------------------------------------------------------------
void loop()
{
  digitalWrite(PWRLEDPin, HIGH);
  emon.calc(20,2000);              //Energy Monitor calc function 
  
  createJSON();
  
  if (DEBUG) {
    transmissionRS();
  }
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
  addJSON(str,"L1",  emon.IrmsL1);
//  addJSON(str,"L2",  emon.IrmsL2);
//  addJSON(str,"L3",  emon.IrmsL3);
  endJSON(str);
}

void transmissionRS()
{
  digitalWrite(RFLEDPin, true);
  Serial.println(str);
  digitalWrite(RFLEDPin, false);
}

void transmissionRF()
{
  digitalWrite(RFLEDPin, true);
  vw_send((uint8_t *)str, strlen(str));
  vw_wait_tx();
  digitalWrite(RFLEDPin, false);
}
