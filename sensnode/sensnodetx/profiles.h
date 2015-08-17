#ifndef PROFILES_H
#define PROFILES_H

/* Defines sensors and functioons per node */

// rooms

#if NODEID == artekroom
	#define DS18B20
	#define SHT21_SENSOR
	#define BMP_SENSOR
#endif

// others

#if NODEID == testnode
	#define DS18B20
	#define OLED
	#define AIRQ
#endif

#endif //__PROFILES_H