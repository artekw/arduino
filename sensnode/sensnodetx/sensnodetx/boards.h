#ifndef BOARDS_H
#define BOARDS_H

// Input/Output definition

#if BOARD_REV == 4
  #define BAT_VOL           6
  #define ONEWIRE_DATA      8
  #define ACT_LED           9
  
  Port p1 (1); // JeeLabs Port P1
  Port p2 (2); // JeeLabs Port P2
  Port p3 (3); // JeeLabs Port P3
  Port p4 (4); // JeeLabs Port P4
#endif // 4.0

#if BOARD_REV == 3
  #define LDR_PIN           2
  #define BAT_VOL           3
  #define MOSFET            7
  #define ONEWIRE_DATA      8
  #define ACT_LED           9
  
  Port p1 (1); // JeeLabs Port P1
  Port p2 (2); // JeeLabs Port P2
#endif // 3.4

#if BOARD_REV < 3
  #define LDR_PIN           0
  #define BAT_VOL           1
  #define CustomA3          3
  #define CustomD3          3
  #define CustomD4          4
  #define MOSFET            5
  #define ONEWIRE_DATA      8
  #define ACT_LED           9

  // No JeeLabs Ports
  
#endif // 3.0


#endif //__BOARDS_H
