// pomiar napiecia przed stabilizatorem
int battVol()
{
  if (!pinState(MOSFET_GATE)) 
  {
     mosfet(0);
  }
//  digitalWrite(BatteryPin, 1);
  int BatteryVal = analogRead(BatteryPin);
  pomiar.battvol = map((BatteryVal), 0, 1023, 0, 660);
//  digitalWrite(BatteryPin, 0);
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
//  digitalWrite(LDRPin, 1);
  int LDRVal = analogRead(LDRPin);
  LDRVal = 1023 - LDRVal;
  pomiar.light = map((LDRVal), 0, 1023, 0, 255);
//  digitalWrite(LDRPin, 0);
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
  activityLed(1);
  rf12_sleep(-1);
  while (!rf12_canSend())
    rf12_recvDone();
  rf12_sendStart(0, &pomiar, sizeof pomiar);
  rf12_sendWait(RADIO_SYNC_MODE);
  rf12_sleep(0);
  activityLed(0);
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

static byte solarOn(int ldr)
{
  byte tmp;
  if (ldr < 220) {
    mosfet(0);
    tmp = 0;
  }
  else
  {
    mosfet(1);
    tmp = 1;
  }
  return pomiar.solar = tmp;
}

static byte pinState(int pin)
{
  return digitalRead(pin) ? 1 : 0;
}
