// GPS Logger v4
// Artur Wronowski <arteqw@gmail.com>

#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SD.h>

// definicja pinów

#define START_BTN 4
#define SYS_LED 6
#define ENGPS_LED 9
#define RX_GPS 3
#define TX_GPS 2
#define BATVOL 0
#define CS 10

SoftwareSerial GPS_Serial(RX_GPS, TX_GPS);
TinyGPS gps;

bool newData = false;
unsigned long chars;
unsigned short sentences, failed;
static byte batvol;

File myFile;

void setup() {
  GPS_Serial.begin(9600);
  Serial.begin(9600);  
  Serial.println("Initializing SD card...");
  
  pinMode(CS, OUTPUT);
  pinMode(ENGPS_LED, OUTPUT); 
  digitalWrite(ENGPS_LED, LOW);
  
  if (!SD.begin(CS)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  pinMode(SYS_LED, OUTPUT);
  digitalWrite(SYS_LED, HIGH);
  
}

void loop() {
  digitalWrite(ENGPS_LED, HIGH);
  
 
  while (GPS_Serial.available())
  {
    char c = GPS_Serial.read();
    if (gps.encode(c))
      newData = true;  
  }
  
    if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);

    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 4);
    Serial.print(" LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 4);
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
    Serial.print(" BATVOL=");
    batvol = analogRead(BATVOL);
    Serial.print(batvol);
    Serial.println("");
    
    myFile = SD.open("test.txt", FILE_WRITE);
    if (myFile) {
      myFile.println(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 4);
    }
    else {
      Serial.println("error opening test.txt");
    }
    delay(1000); 
    myFile.close();
  }
}
