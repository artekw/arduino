static void doMeasure() {
#if I2C
    SHT21.readSensor();
    delay(32);
    BMP085.readSensor();
    getSHT21('h');
    getSHT21('t');
    getBMP085();
#endif
#if ONEWIRE
    getTemp_1Wire();
#endif
#if NTC
  getNTC();
#endif
#if LDR 
  getLDR();
#endif
#if SOLAR_V1
  solar_v1(getLDR());
  measure.bat = 0; // not used
#endif
  getBatVol();
  measure.nodeid = NODEID;
  measure.lobat = rf12_lowbat();
}
/*
float getNTC() {
  int ntc = analogRead(WindPin);
  measure.ntc = ntc;
  return measure.ntc;
}
*/
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
  measure.battvol = map((BatteryVal), 0, 1023, 0, 660);
//  digitalWrite(BatteryPin, 0);
#if SOLAR
  mosfet(MOSFET_SOL, state);
#endif
  return measure.battvol;
}

float getSHT21(char opt)
{
  if (opt == 'h' )
  {
    measure.humi = SHT21.humi;
  }
  else if (opt == 't')
  {
    measure.temp = SHT21.temp;
  }  
  return measure.humi || measure.temp;
}

float getBMP085()
{
  measure.pressure = (BMP085.press*10) + 16;
  return measure.pressure;
}

int getLDR()
{
//  digitalWrite(LDRPin, 1);
  int LDRVal = analogRead(LDRPin);
//  digitalWrite(LDRPin, 0);
  LDRVal = 1023 - LDRVal;
  measure.light = map((LDRVal), 0, 1023, 0, 100);
  return measure.light;
}  

float getTemp_1Wire()
{
#if ONEWIRE
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  return measure.temp = tempC;
#endif
}
