#include <aJSON.h>
#include <LedControl.h>
#include <VirtualWire.h>

LedControl lc=LedControl(2,3,4,1);

#define RFPIN 11
#define RFLED 13

#define CHAR 100
//#define VW_MAX_MESSAGE_LEN 60
//#define DELAY 250
#define DEBUG true

char received[CHAR];

void setup()
{
    pinMode(RFLED, OUTPUT);
    
  if (DEBUG) {
    Serial.begin(9600);	// Debugging only
    Serial.println("DEBUG MODE");
  }
    lc.shutdown(0,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(0,8);
    /* and clear the display */
    lc.clearDisplay(0);
    
 //   vw_set_ptt_inverted(true);
    vw_setup(1200);
    vw_set_rx_pin(RFPIN); 
    vw_rx_start();

}

void loop()
{
 // rec();
  
  aJsonObject* root = aJson.parse(rec());
  aJsonObject* V = aJson.getObjectItem(root, "V");
  if (V) {
    Serial.print("Napiecie: ");
    int nap = atoi(V->valuestring);
    Serial.println(nap);
    lc.shutdown(0,false);
    dValue(nap);
    
  }
  aJson.deleteItem(root);
  
  //dValue(123);
  //delay(2000);
  
}

char* rec() 
{
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    memset(received, 0, sizeof(received)); // czyscimy tablice
    
    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
	int i;

        digitalWrite(RFLED, true);
	for (i = 0; i < buflen; i++)
	{
            received[i]=buf[i];
	}
        if (DEBUG) {
	  Serial.println(received);
        }
        digitalWrite(RFLED, false);
        
        return received;
    }
}

void dValue(int digit) {
  
  int jedn, dzies, setki, tysice;
  
  jedn = digit % 10;
  digit = digit / 10;
  dzies = digit % 10;
  digit = digit / 10;
  setki = digit;
  
  lc.setDigit(0,1,(byte)setki,false);
  lc.setDigit(0,0,(byte)dzies,false);
  lc.setDigit(0,2,(byte)jedn,false);
}
