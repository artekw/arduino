int sensorPin = 0;
int ana;

void setup() {
  Serial.begin(9600);
}

void loop() {
  ana = analogRead(sensorPin);
  float tempC = (3.3 * ana * 100.0)/1024.0; 
  Serial.println(tempC);
  delay(1000);                  
}
