int sensorPin = 1;
int ledPin = 11;
int brig;
int anal;

void setup() {
  Serial.begin(9600);
  Serial.print("Fotorezystor\n");
}

void loop() {
  anal = analogRead(sensorPin);
//  tempC = (5.0 * anal * 100.0/1024.0); 
  Serial.print("Anlogowo: ");
  Serial.println((long)anal);
  anal = 1023 - anal;
  brig = map(anal, 0, 1023, 0, 255);
 // Serial.println((long)brig);
  analogWrite(ledPin, brig);
  delay(100);                  
}
