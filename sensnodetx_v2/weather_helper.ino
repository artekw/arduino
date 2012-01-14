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
#if LDR
  getLDR();
#endif
#if SOLAR
  getSolVol();
  solar(getLDR());
#endif
  getBatVol();
  measure.nodeid = NODEID;
//  measure.lobat = rf12_lowbat();
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

float getWind()
{
  //
}

float getTemp_1Wire()
{
#if ONEWIRE
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  return measure.temp = tempC;
#endif
}
