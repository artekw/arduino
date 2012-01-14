#include "LedControl.h"
#include <Wire.h>
#include <stdio.h>
#include <PCF8583.h>

LedControl lc=LedControl(12,11,10,1); // lc is our object
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219

byte correct_address = 0;
PCF8583 p (0xA0);

byte jedn, dzies, setki, tys = 0;
boolean dp = false;

void setup()
{
	lc.shutdown(0,false);// turn off power saving, enables display
	lc.setIntensity(0,5);// sets brightness (0~15 possible values)
	lc.clearDisplay(0);// clear screen
	Serial.begin(9600);
}

void loop() {
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

	p.get_time();
	char time[50];
	sprintf(time, "%02d/%02d/%02d %02d:%02d:%02d", p.year, p.month, p.day, p.hour, p.minute, p.second);
	Serial.println(time);

	if ( p.hour >= 20 ) {
		lc.setIntensity(0,2);
		}
	else{
		lc.setIntensity(0,15);
		}
	
	jedn = p.minute % 10;
	p.minute = p.minute / 10;
	dzies = p.minute % 10;
	
	setki = p.hour % 10;
	p.hour = p.hour / 10;
	tys = p.hour % 10;

	if ( ( p.second % 2 ) == 0 ) {
		dp = false;
		}
	else {
		dp = true; 
		}
		
	lc.setDigit(0,0,(byte)jedn,false);
	lc.setDigit(0,1,(byte)dzies,false);
	lc.setDigit(0,2,(byte)setki,dp);
	lc.setDigit(0,3,(byte)tys,false);
	
	delay(1000);
}