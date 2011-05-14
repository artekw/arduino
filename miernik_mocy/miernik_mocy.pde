//----------------------------------------------------------------------------
// Mains AC Non Invasive 3
// Last revision 30 November 2009
// Licence: GNU GPL
// By Trystan Lea
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Load Energy Monitor library and create new instance
//----------------------------------------------------------------------------
#include "Emon.h"    //Load the library
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
EnergyMonitor emon;  //Create an instance

//----------------------------------------------------------------------------
// Setup
//----------------------------------------------------------------------------
void setup()
{  
  emon.setPins(0,1);                    //Energy monitor analog pins / V, I
  //emon.calibration(0.9331, 0.1307, 2.3);  //Energy monitor calibration / V, I, F
  emon.calibration(1.025, 0.140025173, 1);  //Energy monitor calibration / V, I, F
  Serial.begin(9600);
  lcd.begin(20, 4);
  lcd.setCursor(3,1);
  lcd.print("MONITOR ENERGII");
  delay(1500);
}

//----------------------------------------------------------------------------
// Main loop
//----------------------------------------------------------------------------
void loop()
{

  emon.calc(20,2000);              //Energy Monitor calc function
  
  Serial.print(emon.realPower);    //Print energy monitor variables
  Serial.print(' ');
  Serial.print(emon.apparentPower);
  Serial.print(' ');
  Serial.print(emon.powerFactor);
  Serial.print(' ');
  Serial.print(emon.Vrms);
  Serial.print(' ');
  Serial.println(emon.Irms);
  
lcd.setCursor(0, 0);
lcd.print("Moc pozorna :  ");
lcd.print(emon.apparentPower);
lcd.setCursor(0, 1);
lcd.print("Moc czynna  :  ");
lcd.print(emon.realPower);
lcd.setCursor(0, 2);
lcd.print("Wsp. mocy   :  ");
lcd.print(emon.powerFactor);

lcd.setCursor(0, 3);
lcd.print("V: ");
lcd.print(emon.Vrms);
lcd.setCursor(10, 3);
lcd.print("|  I: "); 
lcd.print(emon.Irms);

  delay(1000);
 // lcd.clear();
}
//----------------------------------------------------------------------------


