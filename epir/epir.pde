
#include <NewSoftSerial.h>

NewSoftSerial ss(2, 3);

void setup()  
{
  Serial.begin(57600);
  Serial.println("\n[ePIR]");
  ss.begin(9600);
  pinMode(13, OUTPUT);
}

static char getch() {
  while (!ss.available())
  ;
  return ss.read();
}
void loop()                     
{
  ss.print('a');
//  Serial.print(getch());
//  Serial.print(' ');
  if (getch() == 'Y') {
    digitalWrite(13, HIGH);
  }
  else {
    digitalWrite(13, LOW);
  }
  ss.print('b');
  Serial.print((uint8_t) getch(), DEC);
  Serial.println();
  
  delay(1000);
}
