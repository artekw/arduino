#include <SD.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <Wire.h> // necessary, or the application won't build properly
#include <stdio.h>
#include <PCF8583.h>

#define chipSelect 10 // pin 10 in SensnodeTX
#define ONE_WIRE_BUS 8

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int correct_address = 0;
PCF8583 p (0xA0);

void setup()
{
  Serial.begin(9600);
  sensors.begin();
//  Serial.print("Initializing SD card...");
  pinMode(chipSelect, OUTPUT);
  
  if (!SD.begin(chipSelect)) {
//    Serial.println("Card failed, or not present");

    return;
  }
//  Serial.println("card initialized.");
}

void loop()
{
  
  if(Serial.available() > 0){
       p.year= (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48)) + 2000;
       p.month = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
       p.day = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
       p.hour  = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
       p.minute = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
       p.second = (byte) ((Serial.read() - 48) * 10 + (Serial.read() - 48)); // Use of (byte) type casting and ascii math to achieve result.  

       if(Serial.read() == ';'){
         Serial.println("setting date");
	 p.set_time();
       }
  }
  
  // make a string for assembling the data to log:
  String dataString = "";
  
  p.get_time();
  char time[50];
  char buf[50];
  sprintf(time, "%02d/%02d/%02d %02d:%02d:%02d",
	  p.year, p.month, p.day, p.hour, p.minute, p.second);
  sensors.requestTemperatures(); 
  
  dataString = String(time);
  dataString += ",";  
  dtostrf(sensors.getTempCByIndex(0), 2, 2, buf);  
  dataString += buf;
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
//    Serial.println(dataString);
  }  
  // if the file isn't open, pop up an error:
  else {
//    Serial.println("error opening datalog.txt");
  }
  delay(5000);
}









