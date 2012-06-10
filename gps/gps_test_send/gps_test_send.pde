char incomingByte = 0;

int initgps = 0;

// GGA-Global Positioning System Fixed Data, message 103,00
#define LOG_GGA 0
#define GGA_ON   "$PMCAG,005,1,GGA,001\r\n"
#define GGA_OFF  "$PMCAG,005,1,GGA,000\r\n"

// GLL-Geographic Position-Latitude/Longitude, message 103,01
#define LOG_GLL 0
#define GLL_ON   "$PMCAG,005,1,GLL,001\r\n"
#define GLL_OFF  "$PMCAG,005,1,GLL,000\r\n"

// GSA-GNSS DOP and Active Satellites, message 103,02
#define LOG_GSA 1
#define GSA_ON   "$PMCAG,005,1,GSA,001\r\n"
#define GSA_OFF  "$PMCAG,005,1,GSA,000\r\n"

// GSV-GNSS Satellites in View, message 103,03
#define LOG_GSV 0
#define GSV_ON   "$PMCAG,005,1,GSV,001\r\n"
#define GSV_OFF  "$PMCAG,005,1,GSV,000\r\n"

// RMC-Recommended Minimum Specific GNSS Data, message 103,04
#define LOG_RMC 1
#define RMC_ON   "$PMCAG,005,1,RMC,001\r\n"
#define RMC_OFF  "$PMCAG,005,1,RMC,000\r\n"

// VTG-Course Over Ground and Ground Speed, message 103,05
#define LOG_VTG 0
#define VTG_ON   "$PMCAG,005,1,VGT,001\r\n"
#define VTG_OFF  "$PMCAG,005,1,VGT,000\r\n"

#define LOG_ZDA 1
#define ZDA_ON   "$PMCAG,005,1,ZDA,001\r\n"
#define ZDA_OFF  "$PMCAG,005,1,ZDA,000\r\n"

void setup()
{
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
 
  Serial.begin(9600);
  
  delay(1000);
  if (LOG_GGA)
    Serial.print(GGA_ON);
  else
    Serial.print(GGA_OFF);
  delay(1000);

  if (LOG_GLL)
    Serial.print(GLL_ON);
  else
    Serial.print(GLL_OFF);
  delay(1000);

  if (LOG_GSA)
    Serial.print(GSA_ON);
  else
    Serial.print(GSA_OFF);
  delay(1000);

  if (LOG_GSV)
    Serial.print(GSV_ON);
  else
    Serial.print(GSV_OFF);
  delay(1000);

  if (LOG_RMC)
    Serial.print(RMC_ON);
  else
    Serial.print(RMC_OFF);
  delay(1000);

  if (LOG_VTG)
    Serial.print(VTG_ON);
  else
    Serial.print(VTG_OFF);
  delay(1000);
  
  if (LOG_ZDA)
    Serial.print(ZDA_ON);
  else
    Serial.print(ZDA_OFF);
  delay(1000);
}

void loop()
{
//  Serial.print("$PMCAG,005,1,GGA,010,RMC,001,VTG,001,ZDA,010\r\n");
//  delay(1000);
     
if (Serial.available() > 0) {
	incomingByte = Serial.read();
}
}
  
