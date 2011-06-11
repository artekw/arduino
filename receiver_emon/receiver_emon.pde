#include <aJSON.h>
#include <LedControl.h>
#include <VirtualWire.h>

LedControl lc=LedControl(2,3,4,1);

#define RFPIN 11
#define RFLED 13

#define CHAR 100
//#define VW_MAX_MESSAGE_LEN 60
#define DELAY 6000
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
    lc.setIntensity(0,3);
    /* and clear the display */
    lc.clearDisplay(0);
    
    vw_setup(1200);
    vw_set_rx_pin(RFPIN); 
    vw_rx_start();

}

void loop()
{ 
  aJsonObject* root = aJson.parse(receiveData());
  aJsonObject* V = aJson.getObjectItem(root, "V");
  aJsonObject* rP = aJson.getObjectItem(root, "rP");
  aJsonObject* pF = aJson.getObjectItem(root, "pF");
  if (V) {
    Serial.print("Napiecie: ");
    float nap = atof(V->valuestring);
    Serial.println(nap);
    lc.shutdown(0,false);
    printNumber(V->valuestring);
  }
  aJson.deleteItem(root);
}

char* receiveData() 
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

void printNumber(char* string) {
    int ones;
    int tens;
    int hundreds;
    int thousands;
    int dval=0;
    int dp=0;

    double v = atof(string);
    //Display double's with decimal points
    //and make sure as much of the display
    //is utilized as possible.
    if (v<10000.0) {dval = (int)(v);}
    if (v<1000.0) {dval = (int)(v * 10); dp = 3;}
    if (v<100.0) {dval = (int)(v * 100); dp = 2;}
    if (v<10.0) {dval = (int)(v * 1000); dp = 1;}
    if (v<1.0) {dval = (int)(v * 1000); dp = 1;}
    if (v<0.0) {dval=0.0;}

    //Make sure that our dval value is in range
    if(dval < 0 || dval > 9999) 
       return;
    
    ones=dval%10;
    dval=dval/10;
    tens=dval%10;
    dval=dval/10;
    hundreds=dval%10;			
    dval=dval/10;
    thousands=dval;
    
    //Now print the number digit by digit
    if (dp==1) lc.setDigit(0,0,(byte)thousands,true); else lc.setDigit(0,0,(byte)thousands,false);
    if (dp==2) lc.setDigit(0,1,(byte)hundreds,true); else lc.setDigit(0,1,(byte)hundreds,false);
    if (dp==3) lc.setDigit(0,2,(byte)tens,true); else lc.setDigit(0,2,(byte)tens,false);
    lc.setDigit(0,3,(byte)ones,false);
}
