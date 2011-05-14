#include "LedControl.h"

int rando;
int i;

LedControl lc=LedControl(12,11,10,1);

const byte bars[8] = {
  B00000001,
  B00000011,
  B00000111,
  B00001111,
  B00011111,
  B00111111,
  B01111111,
  B11111111 };
  
void setup() {
  lc.shutdown(0,false);
  lc.setIntensity(0,0);
  lc.clearDisplay(0);
  
  Serial.begin(115200);
  Serial.println("Matrix has you!");
}

int x = 0; 

void loop() { 
  rando = random(0,7);
  lc.setRow(0,x, bars[rando]);
  delay(150);
  if (x == 8) {
    x = 0;
  }
  x++;
}
