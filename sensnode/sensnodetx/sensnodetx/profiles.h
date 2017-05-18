#ifndef PROFILES_H
#define PROFILES_H

/* Defines:
  * sensors
  * board rev (BOARD_REV variable)
  * wireless module (RFM69 variable)
*/

// rooms

#if NODEID == artekroom
  #define RFM69 0
  #define BOARD_REV 4
	#define DS18B20
	#define SHT21_SENSOR
	#define BMP_SENSOR
#endif

#if NODEID == foliowiec
  #define RFM69 0
  #define BOARD_REV 2
  #define DS18B20
#endif

#if NODEID == babcia
  #define RFM69 1
  #define BOARD_REV 4
  #define DS18B20
  #define RELAY
#endif

#if NODEID == garaz
  #define RFM69 0
  #define BOARD_REV 3
  #define SHT21_SENSOR
  #define LPG
#endif

#if NODEID == peper
  #define RFM69 1
  #define BOARD_REV 4
  #define SOIL
#endif

// others


#if NODEID == testnode
  #define RFM69 1
  #define BOARD_REV 4
  #define DS18B20
  #define SHT21_SENSOR
  #define BMP_SENSOR
#endif

#if NODEID == car
  #define RFM69 0
  #define BOARD_REV 4
  #define DS18B20
  #define LED_ON
#endif

#if NODEID == outnode
  #define RFM69 0
  #define BOARD_REV 4
  #define DS18B20
#endif

#endif //__PROFILES_H
