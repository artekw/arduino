double Vrms=240.0;

//Setup variables
int numberOfSamples = 5000;

//Set current input pin
int inPinIL1 = 1;
int inPinIL2 = 2;
int inPinIL3 = 3;

//Current calibration coeficient
//double ICAL = 1.0;
double ICAL = 1;
//double ICAL = 0.20648967551;
//Sample variables
int lastSampleIL1, lastSampleIL2, lastSampleIL3, sampleIL1, sampleIL2, sampleIL3;

//Filter variables
double lastFilteredIL1, lastFilteredIL2, lastFilteredIL3, filteredIL1, filteredIL2, filteredIL3;

//Power calculation variables
double sqIL1, sqIL2, sqIL3, sumIL1, sumIL2, sumIL3;

//Useful value variables
double apparentPower,
       Irms,
       Irms1,
       Irms2,
       Irms3;
      
       
void setup()
{
   Serial.begin(9600); 
}

void loop()
{ 

for (int n=0; n<numberOfSamples; n++)
{

   //Used for offset removal
   lastSampleIL1=sampleIL1;
   lastSampleIL2=sampleIL2;
   lastSampleIL3=sampleIL3;
   
   //Read in current samples.   
   sampleIL1 = analogRead(inPinIL1);
   sampleIL2 = analogRead(inPinIL2);
   sampleIL3 = analogRead(inPinIL3);
   
   //Used for offset removal
   lastFilteredIL1 = filteredIL1;
   lastFilteredIL2 = filteredIL2;
   lastFilteredIL3 = filteredIL3;
  
   //Digital high pass filters to remove 2.5V DC offset.
   filteredIL1 = 0.996*(lastFilteredIL1+sampleIL1-lastSampleIL1);
   filteredIL2 = 0.996*(lastFilteredIL2+sampleIL2-lastSampleIL2);
   filteredIL3 = 0.996*(lastFilteredIL3+sampleIL3-lastSampleIL3);
      
   //Root-mean-square method current
   //1) square current values
   sqIL1 = filteredIL1 * filteredIL1;
   sqIL2 = filteredIL2 * filteredIL2;
   sqIL3 = filteredIL3 * filteredIL3;
   //2) sum 
   sumIL1 += sqIL1;
   sumIL2 += sqIL2;
   sumIL3 += sqIL3;
}

//Calculation of the root of the mean of the current squared (rms)
//Calibration coeficients applied. 
Irms1 = ICAL*sqrt(sumIL1 / numberOfSamples); 
Irms2 = ICAL*sqrt(sumIL2 / numberOfSamples);
Irms3 = ICAL*sqrt(sumIL3 / numberOfSamples);

Irms = Irms1 + Irms2 + Irms3;

//Calculation power values
apparentPower = Vrms * Irms;

//Output to serial
//Serial.print(apparentPower);
//Serial.print(' ');
Serial.println("Suma:");
Serial.print(sumIL1);
Serial.print(' ');
Serial.print(sumIL2);
Serial.print(' ');
Serial.println(sumIL3);

Serial.println("Filtr:");
Serial.print(filteredIL1);
Serial.print(' ');
Serial.print(filteredIL2);
Serial.print(' ');
Serial.println(filteredIL3);

Serial.print(Irms1);
Serial.print(' ');
Serial.print(Irms2);
Serial.print(' ');
Serial.print(Irms3);
Serial.print(' ');
Serial.println(Irms);

delay(1000);
//Reset current accumulator
sumIL1 = 0; 
sumIL2 = 0;
sumIL3 = 0;
}
