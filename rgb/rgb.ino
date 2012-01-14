
static byte red = 3;
static byte green = 5;
static byte blue = 6;

void setup()
{
  Serial.begin(9600);
  pinMode(red, OUTPUT);
}

void loop() 
{
  mixColor(100, 0, 100);
}

void mixColor(byte r, byte g, byte b)
{
  analogWrite(red, r);
  analogWrite(green, g);
  analogWrite(blue, b);
}

