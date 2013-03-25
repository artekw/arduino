#include <Button.h>

//create a Button object at pin 12
/*
|| Wiring:
|| GND -----/ ------ pin 12
*/
Button button = Button(12, BUTTON_PULLUP_INTERNAL, true, 10);

byte tmp;
void setup(){
  pinMode(3,OUTPUT); //debug to led 13
}

void loop(){
  if(button.isPressed()){
    if (tmp == 1) {
        digitalWrite(3, LOW);
        tmp = 0;
        return;
    }
    digitalWrite(3,HIGH);
    tmp = 1;
  }else{
    //digitalWrite(3,LOW);
  }
}