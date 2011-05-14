int sensorPin = 0;
int ledPin = 11;
int analogowo;
int brightness;
int tempC;

void setup() {
  Serial.begin(9600);
  Serial.print("LM35\n");
}

void loop() {
  analogowo = analogRead(sensorPin);
  tempC = (5.0 * analogowo * 100.0/1024.0); 
  Serial.print("Anlogowo: ");
  Serial.print((long)analogowo);
  Serial.print(" / Konwersja:");
  Serial.println((long)tempC);
  brightness = map(tempC, 15, 30, 0, 255);
  digitalWrite(ledPin, brightness);
  Serial.println((long)brightness);
  delay(100);                  
}
