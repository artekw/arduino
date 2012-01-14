
int bv = A1;
int sv = A2;

void setup() {
  Serial.begin(115200);
  
}

void loop() {
  int bv_val = analogRead(bv);
  int sv_val = analogRead(sv);
  Serial.println(map((bv_val), 0, 1023, 0, 660));
  Serial.println(map((sv_val), 0, 1023, 0, 660));
  delay(1000);
}
