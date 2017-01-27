#ifndef CONFIGURATION_H
#define CONFIGURATION_H

//************ RADIO MODULE *************
#define NODEID              babcia    // name of node - look nodes.h
#define NETWORK             210
#define BAND                433   // (433 or 868)
// #define LOWRATE                // use on sensbase too !!

//*********** SETTINGS ******************
//#define BOARD_REV           4     // (3.0 or 4.0)
#define RADIO_SYNC_MODE     1     // http://jeelabs.net/pub/docs/jeelib/RF12_8h.html#a6843bbc70df373dbffa0b3d1f33ef0ae

//*********** SENSORS ******************
// Used devices (comment or uncomment if not used)
//#define LDR_SENSOR                    // use LDR sensor
#define DS18B20                  // use 1WIE DS18B20
   #define DS_COUNT           1	 // TO FIX; ONLY 1 - 3 sensors

//#define SHT21_SENSOR           // use SHT21
//#define BMP_SENSOR             // use BMP085 or BMP180
//#define DHT_SENSOR             // DHT11 or DHT22
    #define DHT_SENSOR_TYPE    DHT22
    #define DHT_SENSOR_PORT    P2	 // TODO!!
//#define AIRQ                  	 // Air Quality - MQ-135 eg. FC-22
//#define LPG                      // LPG - MQ-6

// #define RELAY                      // SSR Relay
//*********** OTHER ******************
#define LED_ON                 // use act led for transmission
//#define OLED
//#define FAKE_BAT

//#define DEV_MODE               // display output only at console, do not send packages via radio
//#define DEBUG                  // debug mode - serial output
//#define DEBUG_BAUD      57600  // if not define baud rate = 9600bps

//*********** BATTERY SAVING ********
#define VCC_OK    3400  // enough power for normal 1-minute sends
#define VCC_LOW   3300  // sleep for 1 minute, then try again
#define VCC_DOZE  3200  // sleep for 5 minutes, then try again
                      // sleep for 60 minutes, then try again
#define VCC_SLEEP_MINS(x) ((x) >= VCC_LOW ? 1 : (x) >= VCC_DOZE ? 5 : 60)

#define VCC_FINAL 3100  // send anyway, might be our last swan song

#endif //__CONFIGURATION_H
