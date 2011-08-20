static void doMeasure() {
  solar(getLDR());
  pomiar.lobat = rf12_lowbat();
#if I2C   
    SHT21.readSensor();
    BMP085.readSensor();
#endif
#if I2C
    getSHT21_temp();
    getSHT21_humi();
    getBMP085();
#endif
  getLDR();
  getSolVol();
  getBatVol();
}

int getBatVol()
{
  int BatteryVal = analogRead(BatteryPin);
  pomiar.battvol = map((BatteryVal), 0, 1023, 0, 660);
  return pomiar.battvol;
}

int getSolVol()
{
  int SolarVal = analogRead(SolarPin);
  pomiar.solvol = map((SolarVal), 0, 1023, 0, 660);
  return pomiar.solvol;
}

float getSHT21_humi()
{
   return pomiar.humi = SHT21.humi;
}

float getSHT21_temp()
{
  return pomiar.temp = SHT21.temp;
}

float getBMP085()
{
  pomiar.pressure = (BMP085.press*10) + 16;
  return pomiar.pressure;
}

int getLDR()
{
  int LDRVal = analogRead(LDRPin);
  LDRVal = 1023 - LDRVal;
  pomiar.light = map((LDRVal), 0, 1023, 0, 255);
  return pomiar.light;
}  

float getWind()
{
  //
}

float getTemp()
{
  //
}

static void doReport()
{
  for (byte i = 0; i < RETRY_ACK; ++i) {
    rf12_sleep(-1);
    while (!rf12_canSend())
      rf12_recvDone();
    rf12_sendStart(RF12_HDR_ACK, &pomiar, sizeof pomiar, RADIO_SYNC_MODE);
    byte acked = waitForACK();
    rf12_sleep(0);
    
    if (acked) {
      #if DEBUG
        Serial.print("ACK! ");
        Serial.println((int) i);
        delay(2);
      #endif
      scheduler.timer(MEASURE, MEASURE_PERIOD);
      return;
    }
    Sleepy::loseSomeTime(RETRY_PERIOD * 100);
  }
  scheduler.timer(MEASURE, MEASURE_PERIOD);
  #if DEBUG
    Serial.println("no ACK! ");
    delay(2);
  #endif
}

static void doReport_noack()
{
  rf12_sleep(-1);
  while (!rf12_canSend())
     rf12_recvDone();
  rf12_sendStart(0, &pomiar, sizeof pomiar);
  rf12_sendWait(RADIO_SYNC_MODE);
  rf12_sleep(0);
}

static byte waitForACK() {
  MilliTimer t;
  while (!t.poll(ACK_TIME)) {
    if (rf12_recvDone() && rf12_crc == 0 && rf12_hdr == (RF12_HDR_DST | RF12_HDR_CTL | NODEID))
       return 1;
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_mode();
  }
  return 0;
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
//  Serial.print("SPD ");
//  Serial.println(pomiar.wind);
  Serial.print("LOBAT " );
  Serial.println(pomiar.lobat, DEC);
  Serial.print("BATVOL ");
  Serial.println(pomiar.battvol);
  Serial.print("SOLVOL ");
  Serial.println(pomiar.solvol);
  Serial.print("MSOL ");
  Serial.println(!pinState(MOSFET_SOL), DEC);
  Serial.print("MBAT ");
  Serial.println(!pinState(MOSFET_BAT), DEC);
  activityLed(0);
}

static void activityLed (byte on) {
  pinMode(ACT_LED, OUTPUT);
  digitalWrite(ACT_LED, on);
  delay(150);
}

static void mosfet(byte fet, byte on) {
  pinMode(fet, OUTPUT);
  digitalWrite(fet, !on);
  delay(100);
}

static byte solar(int ldr)
{
  byte tmp;
  if (ldr > SOLAR) {
    mosfet(MOSFET_SOL, 1);
    mosfet(MOSFET_BAT, 1);
    tmp = 1;
  }
  else
  {
    mosfet(MOSFET_SOL, 0);
    mosfet(MOSFET_BAT, 0);
    tmp = 0;
  }
  return (pomiar.solar = tmp, pomiar.bat = tmp);
}

static byte pinState(int pin)
{
  return digitalRead(pin) ? 1 : 0;
}
