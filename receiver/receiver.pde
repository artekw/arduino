#include <VirtualWire.h>

char received[250];

void setup()
{
    Serial.begin(9600);	// Debugging only
    Serial.println("setup");

    vw_set_ptt_inverted(true);
    vw_setup(2000);	 // Bits per sec
    vw_set_rx_pin(11); 
    vw_rx_start();
    pinMode(13, OUTPUT);
}

void loop()
{
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

for(int i = 0;i<249;i++)
{
   received[i] = '\0'; // clearing buffer
}
    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
	int i;

        digitalWrite(13, true); // Flash a light to show received good message
	for (i = 0; i < buflen; i++)
	{
            received[i]=buf[i];
//	    Serial.print(buf[i]);
//            Serial.print(buf[1]);
//            Serial.print(buf[2]);
//            Serial.println(buf[3]);
	}
	Serial.println(received);
        digitalWrite(13, false);
    }
}
