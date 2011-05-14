// include the library code:
#include <LiquidCrystal.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 7
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  
  Serial.begin(9600);
  
  // set up the LCD's number of rows and columns: 
  lcd.begin(20, 4);
  // Print a message to the LCD.
  //lcd.print("hello, world!");
  
   sensors.begin();
}

void loop() {
  sensors.requestTemperatures();
  lcd.clear();
  lcd.print(sensors.getTempCByIndex(0));
}

