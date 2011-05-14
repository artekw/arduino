byte inc;
int ledpin = 12;

void setup() {
  Serial.begin(9600);
  pinMode(ledpin, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    inc = Serial.read() % 2;
  }
    if ( inc == 1 ) 
    {
      blink(ledpin);
    }
    else
    {
      digitalWrite(ledpin, LOW);
    }
}

void blink(int ledpin) {
  digitalWrite(ledpin, HIGH);
  delay(200);
  digitalWrite(ledpin, LOW);
  delay(200);
}
