//PWM pins
byte RED = 9;
byte BLUE = 10;
byte GREEN = 11;
byte COLORS[] = {RED, BLUE, GREEN};

void setup()
{
pinMode(RED, OUTPUT);
pinMode(BLUE, OUTPUT);
pinMode(GREEN, OUTPUT);
}


void loop()
{
int i = 0;
boolean dir = true;

while(1)
{

  fade(COLORS[i%3], dir);
  i++;
  dir = !dir;
}
}
void fade(byte Led, boolean dir)
{
int i;
if(dir) {
   for (i = 0; i<256; i++) {
   analogWrite(Led, i);
   delay(15);
}
}
else {
   for (i = 255; i>=0; i--) {
   analogWrite(Led, i);
   delay(15);
   }
}
}
