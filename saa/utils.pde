void clear_display()
{
  Wire.beginTransmission(display_add1); // transmit to device 1
  Wire.write(0x00);            // writes instruction byte  
  Wire.write(B01000111);             // writes controldata value byte 
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();     // stop transmitting
  delay(50);
  Wire.beginTransmission(display_add2); // transmit to device 2
  Wire.write(0x00);            // writes instruction byte  
  Wire.write(B01000111);             // writes controldata value byte  
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();     // stop transmitting
  delay(50);

}

void display_num(byte chip_add,byte display,byte numero)
{
  Wire.beginTransmission(chip_add); // transmit to device 
  Wire.write((byte)display);
  switch(numero) {
  case 1: 
    {
      Wire.write(UNO);    
      break; 
    }
  case 2:
    {
      Wire.write(DUE);        
      break; 
    }
  case 3:
    {
      Wire.write(TRE);    
      break; 
    }
  case 4:
    {
      Wire.write(QUATTRO);    
      break; 
    }
  case 5:
    {
      Wire.write(CINQUE);    
      break; 
    }
  case 6:
    {
      Wire.write(SEI);    
      break; 
    }
  case 7:
    {
      Wire.write(SETTE);    
      break; 
    }
  case 8:
    {
      Wire.write(OTTO);    
      break; 
    }
  case 9:
    {
      Wire.write(NOVE);    
      break; 
    }
  case 0:
    {
      Wire.write(ZERO);
      break; 
    }
  default:
    {
      Wire.write(0);
    }

  }  
  Wire.endTransmission();     // stop transmitting
}
