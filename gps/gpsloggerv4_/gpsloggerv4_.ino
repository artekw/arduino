
// GPS Logger v4
// Artur Wronowski <arteqw@gmail.com>

#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SD.h>
#include <JeeLib.h>

// definicja pinów

#define START_BTN 4
#define SYS_LED 6
#define ENGPS_LED 9
#define RX_GPS 3
#define TX_GPS 2
#define BATVOL 0
#define CS 10
#define BUFFSIZ 90 // plenty big
byte sleep = 2;

SoftwareSerial GPS_Serial(RX_GPS, TX_GPS);
TinyGPS gps;

bool newData = false;
unsigned long chars;
unsigned short sentences, failed;
static byte batvol;

File myFile;
char buffer[BUFFSIZ];
char buffidx;
uint8_t fix = 0;

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup() {
  
  GPS_Serial.begin(9600);
  Serial.begin(9600);  
  Serial.println("GPSLogger v4");
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
  digitalWrite(ENGPS_LED, HIGH);
  
}

void loop() {
  readline();
  
  if (strncmp(buffer, "$GPRMC",6) == 0) {
    
    char *p = buffer;
    p = strchr(p, ',')+1;
    p = strchr(p, ',')+1;       // skip to 3rd item
    
    if (p[0] == 'V') {
      fix = 0;
      Serial.print("_");
    } else {
      fix = 1;
    }
  }
  
  if (!fix) {
    return;
  } else {
    digitalWrite(SYS_LED, HIGH);
  }
  
   myFile = SD.open("test.txt", FILE_WRITE);
   if (myFile) {
     myFile.println(buffer);
     Serial.println(buffer);
   }
   else {
     Serial.println("error opening test.txt");
   }
  
  myFile.close();
  
  for(int i = 0;i<89;i++)
{
   buffer[i] = '\0'; // clearing buffer
}

  Sleepy::loseSomeTime(60000);

  digitalWrite(SYS_LED, LOW);
  
//  return;
}

void readline(void) {
  char c;
 
  buffidx = 0; // start at begninning
  while (1) {
      c=GPS_Serial.read();
      if (c == -1)
        continue;
//      Serial.print(c);
      if (c == '\n')
        continue;
      if ((buffidx == BUFFSIZ-1) || (c == '\r')) {
        buffer[buffidx] = 0;
        return;
      }
      buffer[buffidx++]= c;
  }
}

void ledcode (byte code) {
  switch (code) {
    case 1: // SD Write
      digitalWrite(SYS_LED, HIGH);
      delay(200);
      digitalWrite(SYS_LED, LOW);
      delay(200);
    break;
    case 2: // Low battery
      digitalWrite(SYS_LED, HIGH);
      delay(50);
      digitalWrite(SYS_LED, LOW);
      delay(50);
    break;
  }
}

