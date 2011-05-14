#include "floatToString.h"

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.println(floatToChar(0.1, 1));
  
  delay(2000);
}

char* floatToChar(double number, int digits)
{
  char finishedChar[20];
  char intPartStr[20];
  char decimalValueStr[20];
  
  // Handle negative numbers
  if (number < 0.0)
  {
    //put a - on the string
     finishedChar[0]='-';
//     Serial.print("\n FinishedChar after minus: "); Serial.print(finishedChar);
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
  {
    rounding /= 10.0;
  }
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  //at the integer part to the string
  itoa(int_part, intPartStr, 10);
  strcat(finishedChar, intPartStr);
//  Serial.print("\n finishedChar after intPart: "); Serial.print(finishedChar);

  // Add a decimal point, but only if there are digits beyond
  if (digits > 0)
  {
    strcat(finishedChar, ".");
//    Serial.print("\n finishedChar after decimal: "); Serial.print(finishedChar);
  }

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    itoa(toPrint, decimalValueStr, 10);
    strcat(finishedChar, decimalValueStr);
//    Serial.print("\n finishedChar remainder: "); Serial.print(finishedChar); Serial.print(" decimal value: "); Serial.print(decimalValueStr);
    remainder -= toPrint; 
  } 

  return finishedChar;

}
