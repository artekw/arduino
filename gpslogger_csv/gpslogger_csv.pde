// this is a generic logger that does checksum testing so the data written should be always good
// Assumes a sirf III chipset logger attached to pin 0 and 1

#include "AF_SDLog.h"
#include "util.h"
#include <avr/pgmspace.h>

#define isdigit(x) ( x >= '0' && x <= '9')

extern uint16_t _end;

AF_SDLog card;
File f;

#define led1Pin 4                // LED1 connected to digital pin 4
#define led2Pin 3                // LED2 connected to digital pin 3
#define powerpin 2               // GPS power control

// set the  RX_BUFFER_SIZE to 32!
#define BUFFSIZE 73         // we buffer one NMEA sentense at a time, 83 bytes is longer than the max length
char buffer[BUFFSIZE];      // this is the double buffer
char buffer2[12];

uint8_t bufferidx = 0;
uint32_t tmp;
#define LOG_RMC  0      // essential location data
#define RMC_ON   "$PSRF103,4,0,1,1*21\r\n"  // the command we send to turn RMC on (1 hz rate)
#define RMC_OFF  "$PSRF103,4,0,0,1*20\r\n"  // the command we send to turn RMC off

#define LOG_GGA  0      // contains fix, hdop & vdop data
#define GGA_ON   "$PSRF103,0,0,1,1*25\r\n"   // the command we send to turn GGA on (1 hz rate)
#define GGA_OFF  "$PSRF103,0,0,0,1*24\r\n"   // the command we send to turn GGA off

#define LOG_GSA 0      // satelite data
#define GSA_ON   "$PSRF103,2,0,1,1*27\r\n"   // the command we send to turn GSA on (1 hz rate)
#define GSA_OFF  "$PSRF103,2,0,0,1*26\r\n"   // the command we send to turn GSA off

#define LOG_GSV  1      // detailed satellite data
#define GSV_ON   "$PSRF103,3,0,1,1*26\r\n"  // the command we send to turn GSV on (1 hz rate)
#define GSV_OFF  "$PSRF103,3,0,0,1*27\r\n"  // the command we send to turn GSV off

#define LOG_GLL 0      // Loran-compatibility data
// this isnt output by default

#define USE_WAAS   0     // useful in US, but slower fix
#define WAAS_ON    "$PSRF151,1*3F\r\n"       // the command for turning on WAAS
#define WAAS_OFF   "$PSRF151,0*3E\r\n"       // the command for turning off WAAS

#define LOG_RMC_FIXONLY 0  // log only when we get RMC's with fix?
uint8_t fix = 0; // current fix data

// read a Hex value and return the decimal equivalent
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

uint8_t i;

// blink out an error code
void error(uint8_t errno) {
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

void setup()                    // run once, when the sketch starts
{
  Serial.begin(9600);
  putstring_nl("GPSlogger");
  pinMode(led1Pin, OUTPUT);      // sets the digital pin as output
  pinMode(led2Pin, OUTPUT);      // sets the digital pin as output
  pinMode(powerpin, OUTPUT);
  digitalWrite(powerpin, LOW);
  
  if (!card.init_card()) {
    putstring_nl("Card init. failed!"); 
    error(1);
  }
  if (!card.open_partition()) {
    putstring_nl("No partition!"); 
    error(2);
  }
  if (!card.open_filesys()) {
    putstring_nl("Can't open filesys"); 
    error(3);
  }
  if (!card.open_dir("/")) {
    putstring_nl("Can't open /"); 
    error(4);
  }
  
  strcpy(buffer, "GPSLOG00.TXT");
  for (buffer[6] = '0'; buffer[6] <= '9'; buffer[6]++) {
      for (buffer[7] = '0'; buffer[7] <= '9'; buffer[7]++) {
         //putstring("\n\rtrying to open ");Serial.println(buffer);
         f = card.open_file(buffer);
         if (!f)
	   break;        // found a file!      
         card.close_file(f);
      }
      if (!f) 
         break;
  }

  if(!card.create_file(buffer)) {
    putstring("couldnt create "); Serial.println(buffer);
    error(5);
  }
  f = card.open_file(buffer);
  if (!f) {
    putstring("error opening "); Serial.println(buffer);
    card.close_file(f);
    error(6);
  }
  putstring("writing to "); Serial.println(buffer);
  putstring_nl("ready!");

  // write header
  strncpy_P(buffer, PSTR("time,lat,long,speed,date,sens0,sens1,sens2\n"), 43);
  Serial.print(buffer);
  
   if(card.write_file(f, (uint8_t *) buffer, 43) != 43) {
       putstring_nl("can't write!");
       return;
   }
 
   delay(1000);

   putstring("\r\n");
#if USE_WAAS == 1 
   putstring(WAAS_ON); // turn on WAAS
#else
  putstring(WAAS_OFF); // turn on WAAS
#endif

#if LOG_RMC == 1
  putstring(RMC_ON); // turn on RMC
#else
  putstring(RMC_OFF); // turn off RMC
#endif

#if LOG_GSV == 1 
  putstring(GSV_ON); // turn on GSV
#else
  putstring(GSV_OFF); // turn off GSV
#endif

#if LOG_GSA == 1
  putstring(GSA_ON); // turn on GSA
#else
  putstring(GSA_OFF); // turn off GSA
#endif

#if LOG_GGA == 1
 putstring(GGA_ON); // turn on GGA
#else
 putstring(GGA_OFF); // turn off GGA
#endif
}

void loop()                     // run over and over again
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
        c = Serial.read(); // wait till we get a $
    }
    buffer[bufferidx] = c;

    //Serial.print(c, BYTE);
    if (c == '\n') {
      //putstring_nl("EOL");
      //Serial.print(buffer);
      buffer[bufferidx+1] = 0; // terminate it
     
      if (buffer[bufferidx-4] != '*') {
        // no checksum?
        Serial.print('*', BYTE);
        bufferidx = 0;
        return;
      }
      // get checksum
      sum = parseHex(buffer[bufferidx-3]) * 16;
      sum += parseHex(buffer[bufferidx-2]);
      
      // check checksum 
      for (i=1; i < (bufferidx-4); i++) {
        sum ^= buffer[i];
      }
      if (sum != 0) {
        //putstring_nl("Cxsum mismatch");
        Serial.print('~', BYTE);
        bufferidx = 0;
        return;
      }
      // got good data!

      if (strstr(buffer, "GPRMC")) {
        // find out if we got a fix
        char *p = buffer;
        p = strchr(p, ',')+1;
        p = strchr(p, ',')+1;       // skip to 3rd item
        
        if (p[0] == 'V') {
          digitalWrite(led1Pin, LOW);
          fix = 0;
        } else {
          digitalWrite(led1Pin, HIGH);
          fix = 1;
        }
      }
#if LOG_RMC_FIXONLY
      if (!fix) {
          Serial.print('_', BYTE);
          bufferidx = 0;
          return;
      } 
#endif
      // rad. lets print it!
      
      Serial.print(buffer);
  
      // time to clean up the string
      // find time
      char *p = buffer;
      p = strchr(p, ',')+1;
      buffer[0] = p[0];
      buffer[1] = p[1];
      buffer[2] = ':';
      buffer[3] = p[2];
      buffer[4] = p[3];
      buffer[5] = ':';
      buffer[6] = p[4];
      buffer[7] = p[5];
      // we ignore milliseconds
      buffer[8] = ',';
      
      p = strchr(buffer+8, ',')+1;
      // skip past 'active' flag
      p = strchr(p, ',')+1;
      // find lat
      p = strchr(p, ',')+1;

      buffer[9] = '+';
      buffer[10] = p[0];
      buffer[11] = p[1];
      buffer[12] = ' ';
      strncpy(buffer+13, p+2, 7);
      buffer[20] = ',';
      
      p = strchr(buffer+21, ',')+1;
      if (p[0] == 'S')
        buffer[9] = '-';
      
      // find long
      p = strchr(p, ',')+1;
      buffer[21] = '+';
      buffer[22] = p[0];
      buffer[23] = p[1];
      buffer[24] = p[2];
      buffer[25] = ' ';
      strncpy(buffer+26, p+3, 7);
      buffer[33] = ',';
      
      p = strchr(buffer+34, ',')+1;
      if (p[0] == 'W')
        buffer[21] = '-';
      
      // find speed
      p = strchr(p, ',')+1;
      tmp = 0;
      if (p[0] != ',') {
        // ok there is some sort of speed
        while (p[0] != '.' && p[0] != ',') {
          tmp *= 10;
          tmp += p[0] - '0';
          p++;       
        }
        tmp *= 10;
        if (isdigit(p[1])) 
          tmp += p[1] - '0'; // tenths
        tmp *= 10;
        if (isdigit(p[2])) 
        tmp += p[2] - '0'; // hundredths
        
        // tmp is knots * 100
        // convert to mph (1.15 mph = 1 knot)
        tmp *= 115;
        // -OR- convert km/h 
        // tmp *= 185
      } 
      tmp /= 100;
      
      buffer[34] = (tmp / 10000) + '0';
      tmp %= 10000;
      buffer[35] = (tmp / 1000) + '0';
      tmp %= 1000;
      buffer[36] = (tmp / 100) + '0';
      tmp %= 100;
      buffer[37] = '.';
      buffer[38] = (tmp / 10) + '0';
      tmp %= 10;
      buffer[39] = tmp + '0';
       
      buffer[40] = ',';
      p = strchr(p, ',')+1;
      // skip past bearing
      p = strchr(p, ',')+1;
      
      // get date into 2001-01-31 style 
      buffer[41] = '2';
      buffer[42] = '0';
      buffer[43] = p[4];
      buffer[44] = p[5];
      buffer[45] = '-';
      buffer[46] = p[2];
      buffer[47] = p[3];
      buffer[48] = '-';
      buffer[49] = p[0];
      buffer[50] = p[1];
      buffer[51] = ',';
      
      // add sensor data after here
      // read analog 0 pin
      tmp = analogRead(0); 
      buffer[52] = (tmp / 1000) + '0'; tmp %= 1000;
      buffer[53] = (tmp / 100) + '0'; tmp %= 100;
      buffer[54] = (tmp / 10) + '0'; tmp %= 10;
      buffer[55] = (tmp) + '0';
      buffer[56] = ',';  
      // read analog 1 pin
      tmp = analogRead(1); 
      buffer[57] = (tmp / 1000) + '0'; tmp %= 1000;
      buffer[58] = (tmp / 100) + '0'; tmp %= 100;
      buffer[59] = (tmp / 10) + '0'; tmp %= 10;
      buffer[60] = (tmp) + '0';  
      buffer[61] = ',';  
       // read analog 2 pin
      tmp = analogRead(2); 
      buffer[62] = (tmp / 1000) + '0'; tmp %= 1000;
      buffer[63] = (tmp / 100) + '0'; tmp %= 100;
      buffer[64] = (tmp / 10) + '0'; tmp %= 10;
      buffer[65] = (tmp) + '0';  
      
      buffer[66] = '\n';
      buffer[67] = 0;
      Serial.print(buffer);
   
      digitalWrite(led2Pin, HIGH);      // sets the digital pin as output

      if(card.write_file(f, (uint8_t *) buffer, strlen(buffer)) != strlen(buffer)) {
         putstring_nl("can't write!");
	 return;
      }
      digitalWrite(led2Pin, LOW);

      bufferidx = 0;
      return;
    }
    bufferidx++;
    if (bufferidx == BUFFSIZE-1) {
       Serial.print('!', BYTE);
       bufferidx = 0;
    }
  }
}

