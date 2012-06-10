// GPS Logger v4
// Artur Wronowski <arteqw@gmail.com>

#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SD.h>s
#include <avr/pgmspace.h>
#include <avr/sleep.h>

// definicja pinów

#define START_BTN 4
#define SYS_LED 6
#define ENGPS_LED 9
#define RX_GPS 3
#define TX_GPS 2
#define BATVOL 0
#define CS 10
#define BUFFSIZ 90 // plenty big
#define SLEEP 2


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
  
  readline();
  
  if (strncmp(buffer, "$GPRMC",6) == 0) {
    
    char *p = buffer;
    p = strchr(p, ',')+1;
    p = strchr(p, ',')+1;       // skip to 3rd item
    
    if (p[0] == 'V') {
      fix = 0;
      Serial.println("no fix");
    } else {
      fix = 1;
  }
  
  if (!fix) {
    return;
  }
  
   digitalWrite(SYS_LED, HIGH);
  
   myFile = SD.open("test.txt", FILE_WRITE);
   if (myFile) {
     myFile.println(buffer);
   }
   else {
     Serial.println("error opening test.txt");
   }
   digitalWrite(SYS_LED, LOW);  
  }
  myFile.close();
  
//  sleep_sec(SLEEP);
//  digitalWrite(SYS_LED, LOW);
  return;
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

void sdwrite() {
  digitalWrite(SYS_LED, HIGH);
  delay(200);
  digitalWrite(SYS_LED, LOW);
  delay(200);
}

void sleep_sec(uint8_t x) {
  while (x--) {
     // set the WDT to wake us up!
    WDTCSR |= (1 << WDCE) | (1 << WDE); // enable watchdog & enable changing it
    WDTCSR = (1<< WDE) | (1 <<WDP2) | (1 << WDP1);
    WDTCSR |= (1<< WDIE);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_mode();
    sleep_disable();
  }
}

SIGNAL(WDT_vect) {
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = 0;
}
