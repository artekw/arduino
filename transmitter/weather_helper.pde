// pomiar napiecia przed stabilizatorem
float battVol()
{
 // mosfet(0);
  pinMode(BatteryPin, INPUT);
  int BatteryVal = analogRead(BatteryPin);
  pomiar.battvol = map(BatteryVal, 0, 1023, 0, 660);
  pinMode(BatteryPin, OUTPUT);
  return pomiar.battvol;
}

// pomiar wilgotnosci lub temperatury
float getSHT21(char opt)
{
  if (opt == 'h' )
  {
    pomiar.humi = SHT21.humi;
  }
  else if (opt == 't')
  {
    pomiar.temp = SHT21.temp;
  }  
  return pomiar.humi || pomiar.temp;
}

// pomiar cisnienia
float getBMP085()
{
  pomiar.pressure = (BMP085.press*10) + 16;
  return pomiar.pressure;
}

// pomiar nasłonecznienia
int getLDR()
{
  pinMode(LDRPin, INPUT);
  int LDRVal = analogRead(LDRPin);
  LDRVal = 1023 - LDRVal;
//  float vo = LDRVal * (3.3 / 1024.0);
//  pomiar.light = (int)500/(ResLDR*((3.3-vo)/vo));
  pomiar.light = map((LDRVal), 0, 1023, 0, 255);
  pinMode(LDRPin, OUTPUT);
  return pomiar.light;
}  

// pomiar prędkości wiatru
float getWind()
{
  //
}

// pomiar temperatury z DS18B20
float getTemp()
{
  //
}

// transmisja RF
static void transmissionRF()
{
  rf12_sleep(-1);
  while (!rf12_canSend())
    rf12_recvDone();
  rf12_sendStart(0, &pomiar, sizeof pomiar, 2);
  rf12_sleep(0);
}

static void transmissionRS()
{
  activityLed(1);
  Serial.print("LIGHT ");
  Serial.println(pomiar.light);
  Serial.print("HUMI ");
  Serial.println(pomiar.humi);
  Serial.print("TEMP ");
  Serial.println(pomiar.temp);
  Serial.print("PRES ");
  Serial.println(pomiar.pressure);
  Serial.print("SPD ");
  Serial.println(pomiar.wind);
  Serial.print("LOBAT " );
  Serial.println(pomiar.lobat, DEC);
  Serial.print("BATVOL ");
  Serial.println(pomiar.battvol);
  Serial.print("MOSFET ");
  Serial.println(!pinState(MOSFET_GATE), DEC);
  Serial.println("-------");
  activityLed(0);
}

static void activityLed (byte on) {
  pinMode(ACT_LED, OUTPUT);
  digitalWrite(ACT_LED, on);
  delay(150);
}

static void mosfet(byte on) {
  pinMode(MOSFET_GATE, OUTPUT);
  digitalWrite(MOSFET_GATE, !on);
  delay(100);
}

static void solarOn(int ldr)
{
  if (ldr < 150) {
    mosfet(0);
  }
  else
  {
    mosfet(1);
  }
}

static byte pinState(int pin)
{
  return digitalRead(pin) ? 1 : 0;
}
