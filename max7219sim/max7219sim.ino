#include "LedControl.h"
// Arduino Pin 7 to DIN, 6 to Clk, 5 to LOAD, no.of devices is 1
LedControl lc=LedControl(12,11,10,1); // lc is our object
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219
void setup()
{
 // Initialize the MAX7219 device
  lc.shutdown(0,false);   // Enable display
  lc.setIntensity(0,10);  // Set brightness level (0 is min, 15 is max)
  lc.clearDisplay(0);     // Clear display register
}
void loop()
{
  for(int i=0; i&lt;8; i++){
   lc.setDigit(0,i,i+1,false);
  }
  delay(1000);
}