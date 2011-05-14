unsigned char p;
boolean printed = 0;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
if (Serial.available()>0)
{
p = Serial.read();
if (p == 2)
{
while (p != 3)
{
delay(5);
p = Serial.read();
Serial.print(p);
}
printed = 1;
delay(150);
}
}
if (printed)
{
Serial.println("\n----------");
printed = 0;
}
}
