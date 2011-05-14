#include "LedControl.h"
LedControl lc=LedControl(12,11,10,1);

void setup() {
  lc.shutdown(0,false);
  lc.setIntensity(0,1);
  lc.clearDisplay(0);
}

const byte a[6]={B01111110,B10001000,B10001000,B10001000,B01111110,B00000000};
const byte r[6]={B00111110,B00010000,B00100000,B00100000,B00010000,B00000000};
const byte d[6]={B00011100,B00100010,B00100010,B00010010,B11111110,B00000000};
const byte u[6]={B00111100,B00000010,B00000010,B00000100,B00111110,B00000000};
const byte i[6]={B00000000,B00100010,B10111110,B00000010,B00000000,B00000000};
const byte n[6]={B00111110,B00010000,B00100000,B00100000,B00011110,B00000000};
const byte o[6]={B00011100,B00100010,B00100010,B00100010,B00011100,B00000000};

void display(const byte letter[6]) {
 int i,j,k = 0;
  for (i=8; i>=0; i--) {
  for (j=0; j<6; j++) {
      lc.setRow(0,j+i,letter[j]);
      }
      delay(150);
}
}

void scroll(char * s) {
  int var=0;
  while (s[var]) {
    if (s[var]=='A')
      display(a);
    if (s[var]=='R')
      display(r);  
    if (s[var]=='D')
      display(d);   
    if (s[var]=='U')
      display(u);   
    if (s[var]=='I')
      display(i); 
    if (s[var]=='N')
      display(n); 
    if (s[var]=='O')
      display(o); 
    var++;
  }
}

void loop() { 

scroll("ARDUINO");
}
