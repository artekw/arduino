
#define volt_pin A0

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(13, OUTPUT);  
  pinMode(volt_pin, INPUT);
  digitalWrite(13, HIGH); 
  Serial.begin(9600); 
}

void loop() {
/*  digitalWrite(13, HIGH);   // set the LED on
  delay(1000);              // wait for a second
  digitalWrite(13, LOW);    // set the LED off
  delay(1000);    */          // wait for a second
  vsupp();
}

void vsupp() {
  float raw;
  raw = analogRead(volt_pin);
  int v = map(analogRead(volt_pin), 0, 1023, 0, 660);
 /*
  float vo = (raw * 5.0) /1024.0;
  float vi = vo / 0.5;
  Serial.print("vo:");
  Serial.println(vo);
  Serial.print("vi:");
  Serial.println(vi); 
  */
  Serial.println(v);
  delay(1000);
}
