#include <NewSoftSerial.h>

#define PWM0 9
#define PWR_LED 12

#define TIME_LIGHT 50000
#define DIMM_OUT_TIME 100

NewSoftSerial ss(3, 2);

void setup() 
{
  pinMode(PWM0, OUTPUT); 
  pinMode(PWR_LED, OUTPUT);
  ss.begin(9600);
}

void loop() 
{
  digitalWrite(PWR_LED, HIGH);
  if (move()) {
    light();
  }
//  Serial.println(move());
  delay(200);
}

void light()
{
  digitalWrite(PWM0, HIGH);
  delay(TIME_LIGHT);
  dimmout(DIMM_OUT_TIME);
}

static int move()
{
  int state;
  ss.print('a');
  if (getch() == 'Y') {
    state = 1; 

  }
  else {
    state = 0;
  }
  return state;
}

void dimmin(int time)
{
  for(int fadeValue = 0 ; fadeValue <= 255; fadeValue +=5) { 
  analogWrite(PWM0, fadeValue);         
  delay(time);                            
  }
}

void dimmout(int time)
{
  for(int fadeValue = 255 ; fadeValue >= 0; fadeValue -=5) { 
  analogWrite(PWM0, fadeValue);         
  delay(time);
  if (move()) {
    light();
    break;
  }
  }
}

void blink()
{
  digitalWrite(PWM0, HIGH);
  delay(200);
  digitalWrite(PWM0, LOW);
  delay(200);
}

static char getch() {
  while (!ss.available())
  ;
  return ss.read();
}
