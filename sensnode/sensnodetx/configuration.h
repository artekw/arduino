#ifndef CONFIGURATION_H
#define CONFIGURATION_H

//************ RADIO MODULE *************
#define NODEID              15  // (from 1 to 30)
#define NETWORK             210
#define NEW_REV                 // comment for old revision  (3.0 or 3.4)

//*********** SETTINGS ******************
//#define SMOOTH            3   // smoothing factor used for running averages
#define PERIOD              1   // minutes
#define RADIO_SYNC_MODE     2   // http://jeelabs.net/pub/docs/jeelib/RF12_8h.html#a6843bbc70df373dbffa0b3d1f33ef0ae

//*********** SENSORS ******************
// Used devices (comment on uncomment if not used)
#define LDR                    // use LDR sensor
#define DS18B20                // use 1WIE DS18B20

#define SHT21_SENSOR           // use SHT21
#define BMP_SENSOR             // use BMP085 or BMP180
#define DHT_SENSOR             // DHT11 or DHT22

//*********** OTHER ******************
#define LED_ON                 // use act led for transmission

//#define DEBUG                  // debug mode - serial output

#endif //__CONFIGURATION_H
