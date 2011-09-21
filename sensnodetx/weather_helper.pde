static void doMeasure() {
#if I2C
    SHT21.readSensor();
    delay(32);
    BMP085.readSensor();
    // measure
    getSHT21('h');
    getSHT21('t');
    getBMP085();
#endif
#if ONEWIRE
    getTemp_1Wire();
#endif
//  pomiar.lobat = rf12_lowbat();
  getBatVol();
  getLDR();
#if SOLAR
  getSolVol();
  solar(getLDR());
#endif
#if SOLAR_V1
  solar_v1(getLDR());
  pomiar.bat = 0; // not used
#endif
//  ++pomiar.seq;
}

int getBatVol()
{
#if SOLAR
  byte state = pinState(MOSFET_SOL);
  if (!pinState(MOSFET_SOL)) 
  {
     mosfet(MOSFET_SOL, 0);
  }
#endif
//  digitalWrite(BatteryPin, 1);
  int BatteryVal = analogRead(BatteryVolPin);
  pomiar.battvol = map((BatteryVal), 0, 1023, 0, 660);
//  digitalWrite(BatteryPin, 0);
#if SOLAR
  mosfet(MOSFET_SOL, state);
#endif
  return pomiar.battvol;
}

int getSolVol()
{

  byte state = pinState(MOSFET_BAT);
  if (!pinState(MOSFET_BAT)) 
  {
     mosfet(MOSFET_BAT, 0);
  }

  int SolarVal = analogRead(SolarVolPin);
  pomiar.solvol = map((SolarVal), 0, 1023, 0, 660);

  mosfet(MOSFET_BAT, state);

  return pomiar.solvol;
}

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

float getBMP085()
{
  pomiar.pressure = (BMP085.press*10) + 16;
  return pomiar.pressure;
}

int getLDR()
{
//  digitalWrite(LDRPin, 1);
  int LDRVal = analogRead(LDRPin);
//  digitalWrite(LDRPin, 0);
  LDRVal = 1023 - LDRVal;
  pomiar.light = map((LDRVal), 0, 1023, 0, 255);
  return pomiar.light;
}  

float getWind()
{
  //
}

float getTemp_1Wire()
{
#if ONEWIRE
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  return pomiar.temp = tempC;
#endif
}
