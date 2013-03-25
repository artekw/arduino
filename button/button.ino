#include <Button.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x38);  // Set the LCD I2C address

Button button1 = Button(7, BUTTON_PULLUP_INTERNAL, true, 100);
Button button2 = Button(6, BUTTON_PULLUP_INTERNAL, true, 100);

#define BACKLIGHT_PIN     13
#define BOILER 	0
#define SOLAR 1
#define PUMP 12

int i = 0;
boolean pump_on = false;

float tank_max_temp = 38;      // Deg. F; above this, pump is off
float collector_min_temp = 20;  // Deg. F; below this, pump is off
float on_threshold = 4;        // Deg. F differential before pump turns on
float off_threshold = 2;       // Deg. F differential before pump turns off


void onPress(Button& b){
	if (b.pin == 6) {
		i = i + 1;
		Serial.println("p2");
	}
	if (b.pin == 7) {
		Serial.println("p1");
		i = i - 1;
	}

}

void onHold(Button& b){
	Serial.print("HOLD: ");
	Serial.println(b.pin);
}

float readTemp(byte pin) {
	float temp;
	temp = (5.0 * analogRead(pin) * 100.0) / 1024;
	return temp;
}

void pump(byte state) {
	digitalWrite(PUMP, state);
	if (state == 1) {
		pump_on = true;
	} else {
		pump_on = false;
	}
}


void control() {
	if (readTemp(BOILER) < -50 || readTemp(SOLAR) < -50) {
    Serial.println("# Screwy reading, ignoring");
  }

  else {

    // If tank is hot enough, pump is off
    if (readTemp(BOILER) >= tank_max_temp) {
      Serial.println("# Tank is over max temp, pump off");
      pump(0);
    }

    // If collector is cold, pump is off
    else if (readTemp(SOLAR) <= collector_min_temp) {
      Serial.println("# Collector is under min temp, pump off");
      pump(0);
    }

    // If collector is significantly hotter thank tank, pump is on
    else if (readTemp(SOLAR) > readTemp(BOILER) + on_threshold) {
      Serial.println("# Collector is hotter than tank, pump on");
      pump(1);
    }

    // If collector is significantly colder than tank, pump is off
    else if (readTemp(SOLAR) < readTemp(BOILER) + off_threshold) {
      Serial.println("# Collector is colder than tank, pump off");
      pump(0);
    }

    // Otherwise, no change
    else {
    }
  }
}
/* main */

void setup(){
  Serial.begin(9600);

  // Assign callback function
  button1.pressHandler(onPress);
  button2.pressHandler(onPress);
  button1.holdHandler(onHold, 1000);
  button2.holdHandler(onHold, 1000);

  pinMode ( BACKLIGHT_PIN, OUTPUT );
  pinMode (PUMP, OUTPUT);

  digitalWrite ( BACKLIGHT_PIN, HIGH );

  lcd.begin(16,2);               // initialize the lcd 
  lcd.home ();                   // go home
  lcd.print("DEMO ");
}

void loop(){
  // update the buttons' internals
  button1.process();
  button2.process();
  control();

  lcd.clear();
 /*
  lcd.setCursor (12, 0);
  lcd.print(i);
*/
  /* b*/
  lcd.setCursor (0, 0);
  lcd.print("B:");
  lcd.setCursor (3, 0);
  lcd.print(readTemp(BOILER));

  /* k */
  lcd.setCursor (0, 1);
	lcd.print("K:");
	lcd.setCursor (3, 1);
	lcd.print(readTemp(SOLAR));
  

  /* p */
  lcd.setCursor(15,1);
  if (pump_on == true) {
  	lcd.print("1");
  }
  else {
  	lcd.print("0");
  }

	delay(1000);
}