int relaypin =  2;
int state = 0;

void setup()   {    

  Serial.begin(115200);
  pinMode(relaypin, OUTPUT);    
  Serial.println("Wpisz 0 lub 1 aby zalaczyc przekaznik");
}

void loop()                     
{
  if (Serial.available() > 0) {
    state = Serial.read();

    switch(state) {
      case '1':
      digitalWrite(relaypin, HIGH);
      break;
      case '0':
      digitalWrite(relaypin, LOW);
      break;
      default:
      Serial.print("Pomylka!");
    }
    delay(5000);
    digitalWrite(relaypin, LOW);
  }
}
