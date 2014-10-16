// Demo for multiple fonts on the Graphics Board
// 2010-05-28 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <GLCD_ST7565.h>
#include <JeeLib.h>
#include <avr/pgmspace.h>

#include "utility/font_clR6x8.h"
#include "utility/font_clR6x6.h"
#include "utility/font_helvBO12.h"
#include "utility/font_helvO12.h"
#include "utility/font_clR4x6.h"
#include "utility/font_ncenBI14.h"
#include "utility/font_ncenR08.h"

GLCD_ST7565 glcd;

extern byte gLCDBuf[1024];

void setup () {
  glcd.begin();
//  glcd.backLight(255);
  
  glcd.setFont(font_clR6x8);
  glcd.drawString_P(84,  0, PSTR("GLCDlib"));
  glcd.drawLine(85, 8, 125, 8, WHITE);
  
  glcd.setFont(font_clR6x6);
  glcd.drawString(0, 4, "0123456789");
  glcd.drawString(0, 10, "ABCDEFGHIJKLMNOPQRSTU");
  glcd.drawString(0, 16, "abcdefghijklmnopqrstu");

  glcd.setFont(font_helvBO12);
  byte x = glcd.drawString(5, 22, "Wide");
  glcd.setFont(font_helvO12);
  glcd.drawString(x, 22, " & narrow...");

  glcd.setFont(font_clR4x6);
  glcd.drawString(2, 37, "The quick brown fox jumps");
  glcd.drawString(5, 43, "over the lazy dog");
  glcd.drawString(8, 49, "0123456789");
  
  glcd.setFont(font_ncenBI14);
  glcd.drawString(44, 44, "...really!");

  glcd.setFont(font_ncenR08);
  glcd.drawString(0, 55, "JeeLabs");
  
  glcd.refresh();
}

void loop () {}
