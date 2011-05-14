#include <SdFat.h>
#include <SdFatUtil.h>
#include <avr/sleep.h>

#define putstring(str) SerialPrint_P(PSTR(str))
#define putstring_nl(str) SerialPrintln_P(PSTR(str))

// oszczednośc energii
#define SLEEPDELAY 5
#define TURNOFFGPS 0
#define LOG_RMC_FIXONLY 1

Sd2Card card;
SdVolume volume;
SdFile root;
SdFile f;

#define led1Pin 3
#define led2Pin 2
#define powerPin 4

#define BUFFSIZE 75
char buffer[BUFFSIZE];
uint8_t bufferidx = 0;
uint8_t fix = 0;
uint8_t i;

// GGA-Global Positioning System Fixed Data
#define LOG_GGA 0
#define GGA_ON   "$PMCAG,005,1,GGA,001\r\n"
#define GGA_OFF  "$PMCAG,005,1,GGA,000\r\n"

// GLL-Geographic Position-Latitude/Longitude
#define LOG_GLL 0
#define GLL_ON   "$PMCAG,005,1,GLL,001\r\n"
#define GLL_OFF  "$PMCAG,005,1,GLL,000\r\n"

// GSA-GNSS DOP and Active Satellites
#define LOG_GSA 1
#define GSA_ON   "$PMCAG,005,1,GSA,001\r\n"
#define GSA_OFF  "$PMCAG,005,1,GSA,000\r\n"

// GSV-GNSS Satellites in View
#define LOG_GSV 0
#define GSV_ON   "$PMCAG,005,1,GSV,001\r\n"
#define GSV_OFF  "$PMCAG,005,1,GSV,000\r\n"

// RMC-Recommended Minimum Specific GNSS Data
#define LOG_RMC 1
#define RMC_ON   "$PMCAG,005,1,RMC,001\r\n"
#define RMC_OFF  "$PMCAG,005,1,RMC,000\r\n"

// VTG-Course Over Ground and Ground Speed
#define LOG_VTG 0
#define VTG_ON   "$PMCAG,005,1,VGT,001\r\n"
#define VTG_OFF  "$PMCAG,005,1,VGT,000\r\n"

// ZDA - Time and Date
#define LOG_ZDA 1
#define ZDA_ON   "$PMCAG,005,1,ZDA,001\r\n"
#define ZDA_OFF  "$PMCAG,005,1,ZDA,000\r\n"

uint8_t parseHex(char c) {
  if (c < '0')
    return 0;
  if (c <= '9')
    return c - '0';
  if (c < 'A')
    return 0;
  if (c <= 'F')
    return (c - 'A')+10;
}

// kod błędów
void error(uint8_t errno) {
  if (card.errorCode()) {
    putstring("SD error: ");
    Serial.print(card.errorCode(), HEX);
    Serial.print(',');
    Serial.println(card.errorData(), HEX);
  }
  while(1) {
    for (i=0; i<errno; i++) {
      digitalWrite(led1Pin, HIGH);
      digitalWrite(led2Pin, HIGH);
      delay(100);
      digitalWrite(led1Pin, LOW);
      digitalWrite(led2Pin, LOW);
      delay(100);
    }
    for (; i<10; i++) {
      delay(200);
    }
  }
}

void setup()
{
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = 0;
  Serial.begin(9600);
  putstring_nl("\r\nGPSlogger Shield v1.0");
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, LOW);

  if (!card.init()) {
    putstring_nl("Card init. failed!");
    error(1);
  }
  if (!volume.init(card)) {
    putstring_nl("No partition!");
    error(2);
  }
  if(!root.openRoot(volume)) {
    putstring_nl("Can't! open root dir");
    error(3);
  }
  strcpy(buffer, "GPSLOG00.TXT");
  for (i = 0; i < 100; i++) {
    buffer[6] = '0' + i/10;
    buffer[7] = '0' + i%10;
    if (f.open(root, buffer, O_CREAT | O_EXCL | O_WRITE | O_SYNC)) break;
  }
  
  if(!f.isOpen()) {
    putstring("couldnt create "); Serial.println(buffer);
    error(3);
  }
  putstring("writing to "); Serial.println(buffer);
  putstring_nl("ready!");
  
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
}

void loop()
{
  //Serial.println(Serial.available(), DEC);
  char c;
  uint8_t sum;

  // read one 'line'
  if (Serial.available()) {
    c = Serial.read();
    //Serial.print(c, BYTE);
    if (bufferidx == 0) {
      while (c != '$')
        c = Serial.read(); // czekamy na $
    }
    buffer[bufferidx] = c;

    //Serial.print(c, BYTE);
    if (c == '\n') {
      //putstring_nl("EOL");
      //Serial.print(buffer);
      buffer[bufferidx+1] = 0;

      if (buffer[bufferidx-4] != '*') {
        // brak sumy ?
        Serial.print('*', BYTE);
        bufferidx = 0;
        return;
      }
      // pobierz sume kontrolną
      sum = parseHex(buffer[bufferidx-3]) * 16;
      sum += parseHex(buffer[bufferidx-2]);

      // sprawdz poprawność sumy
      for (i=1; i < (bufferidx-4); i++) {
        sum ^= buffer[i];
      }
      if (sum != 0) {
        //putstring_nl("Cxsum mismatch");
        Serial.print('~', BYTE);
        bufferidx = 0;
        return;
      }

      if (strstr(buffer, "GPRMC")) {
        // czy mamy fixa ?
        char *p = buffer;
        p = strchr(p, ',')+1;
        p = strchr(p, ',')+1;

        if (p[0] == 'V') {
          digitalWrite(led1Pin, LOW);
          fix = 0;
        } else {
          digitalWrite(led1Pin, HIGH);
          fix = 1;
        }
      }
      
      if(!fix) {
        digitalWrite(led1Pin, HIGH);
        delay(250);
        digitalWrite(led1Pin, LOW);
        delay(250);
      }
      
      if (LOG_RMC_FIXONLY) {
        if (!fix) {
          Serial.print('_', BYTE);
          bufferidx = 0;
          return;
        }
      }
      // logujemy!
      Serial.print(buffer);
      Serial.print('#', BYTE);
      digitalWrite(led2Pin, HIGH);

      bufferidx++;

      if(f.write((uint8_t *) buffer, bufferidx) != bufferidx) {
         putstring_nl("can't write!");
         error(4);
      }

      digitalWrite(led2Pin, LOW);

      bufferidx = 0;

      if (TURNOFFGPS) {
        digitalWrite(powerPin, HIGH);
      }

      sleep_sec(SLEEPDELAY);
      digitalWrite(powerPin, LOW);
      return;
    }
    bufferidx++;
    if (bufferidx == BUFFSIZE-1) {
       Serial.print('!', BYTE);
       bufferidx = 0;
    }
  } else {

  }
  
}

void sleep_sec(uint8_t x) {
  while (x--) {
    WDTCSR |= (1 << WDCE) | (1 << WDE);
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

