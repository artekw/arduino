#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#define ONE_WIRE_BUS 7

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup(void)
{
  lcd.begin(20, 4);
//  Serial.begin(9600);
//  sensors.begin();
  lcd.setCursor(0, 0);
  lcd.print("Dallas DS18B20 Demo");
  lcd.setCursor(11, 3);
  lcd.print("by arteq");
  delay(1500);
}

void loop(void)
{
  sensors.requestTemperatures();
  Serial.println("");

  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Czujnik1:");
  lcd.setCursor(10, 0);
  lcd.print(sensors.getTempCByIndex(0));
  lcd.setCursor(16, 0);
  lcd.print("*C");    
//  Serial.print("Czujnik-1: ");
//  Serial.println(sensors.getTempCByIndex(0));

  lcd.setCursor(0, 1);
  lcd.print("Czujnik2:");
  lcd.setCursor(10, 1);
  lcd.print(sensors.getTempCByIndex(1));
  lcd.setCursor(16, 1);
  lcd.print("*C");  
//  Serial.print("Czujnik-2: ");
//  Serial.println(sensors.getTempCByIndex(1));

  lcd.setCursor(0, 2);
  lcd.print("Czujnik3:");
  lcd.setCursor(10, 2);
  lcd.print(sensors.getTempCByIndex(2));
  lcd.setCursor(16, 2);
  lcd.print("*C");    
//  Serial.print("Czujnik-3: ");
//  Serial.println(sensors.getTempCByIndex(2));

  delay(2000);
}
