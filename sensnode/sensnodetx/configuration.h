#ifndef CONFIGURATION_H
#define CONFIGURATION_H

//************ RADIO MODULE *************
#define NODEID              1  // (from 1 to 30)
#define NETWORK             210
#define NEW_REV                 // comment for old revision  (3.0 or 3.4)
#define BAND RF12_868MHZ // Band
// #define LOWRATE  

//*********** SETTINGS ******************
#define RADIO_SYNC_MODE     2   // http://jeelabs.net/pub/docs/jeelib/RF12_8h.html#a6843bbc70df373dbffa0b3d1f33ef0ae

//*********** SENSORS ******************
// Used devices (comment or uncomment if not used)
//#define LDR                    // use LDR sensor
#define DS18B20                // use 1WIE DS18B20
#define DS_COUNT           3   // TO FIX; NOW ONLY 1 or 3 sensors

//#define SHT21_SENSOR           // use SHT21
#define BMP_SENSOR             // use BMP085 or BMP180
#define DHT_SENSOR             // DHT11 or DHT22
#define DHT_SENSOR_TYPE    DHT22
#define DHT_SENSOR_PORT    P1  // TODO!!

//*********** OTHER ******************
#define LED_ON                 // use act led for transmission

#define DEV_MODE               // display output only on console, do not send packages via radio
//#define DEBUG                  // debug mode - serial output
#define DEBUG_BAUD      57600  // if not define baud rate = 9600bps

#endif //__CONFIGURATION_H
