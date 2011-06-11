const byte nsum=10;

int humidityPin=1;

unsigned int sensorValue2 = 0;  // variable to store the value coming from the sensor

void setup() {
  Serial.begin(9600);
}

void loop() {
  for (byte i=0;i<nsum;i++)
  {    
    sensorValue2 += analogRead(humidityPin);    
  }    
  int sensorValue2Avg=sensorValue2/nsum;
  float RH= 0.1515*sensorValue2Avg;
  
  Serial.print(RH,0);
  Serial.println(" %RH");
  
  sensorValue2=0;
  delay(5000);
}
