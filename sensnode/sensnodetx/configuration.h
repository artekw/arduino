#ifndef CONFIGURATION_H
#define CONFIGURATION_H

//************ RADIO MODULE *************
#define NODEID              2  // (1 to 30)
#define NETWORK             210
//#define NEW_REV                  // comment for old revision  (3.0 or 3.4)
#define RFM69               1    // RFM12 compat mode for RFM69CW
#define BAND                433  // (433 or 868)
// #define LOWRATE               // use on sensbase too !!

//*********** SETTINGS ******************
#define RADIO_SYNC_MODE     2   // http://jeelabs.net/pub/docs/jeelib/RF12_8h.html#a6843bbc70df373dbffa0b3d1f33ef0ae

//*********** SENSORS ******************
// Used devices (comment or uncomment if not used)
//#define LDR_SENSOR                    // use LDR sensor
#define DS18B20                // use 1WIE DS18B20
#define DS_COUNT           1   // TO FIX; NOW ONLY 1 or 3 sensors

//#define SHT21_SENSOR           // use SHT21
//#define BMP_SENSOR             // use BMP085 or BMP180
//#define DHT_SENSOR             // DHT11 or DHT22
#define DHT_SENSOR_TYPE    DHT22
#define DHT_SENSOR_PORT    P1  // TODO!!

//*********** OTHER ******************
#define LED_ON                 // use act led for transmission

#define DEV_MODE               // display output only on console, do not send packages via radio
#define DEBUG                  // debug mode - serial output
#define DEBUG_BAUD      57600  // if not define baud rate = 9600bps


//*********** BATTERY SAVING ********
#define VCC_OK    3400  // enough power for normal 1-minute sends
#define VCC_LOW   3300  // sleep for 1 minute, then try again
#define VCC_DOZE  3200  // sleep for 5 minutes, then try again
                      // sleep for 60 minutes, then try again
#define VCC_SLEEP_MINS(x) ((x) >= VCC_LOW ? 1 : (x) >= VCC_DOZE ? 5 : 60)

#define VCC_FINAL 3100  // send anyway, might be our last swan song

#endif //__CONFIGURATION_H
