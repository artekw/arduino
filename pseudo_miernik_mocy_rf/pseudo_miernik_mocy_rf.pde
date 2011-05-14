char str[150];

void setup()
{  
  Serial.begin(9600);
}

void loop()
{
  createJSON();
  
  Serial.println(str);
  
  delay (5*1000);
  
  for(int i = 0;i<149;i++)
  {
     str[i] = '\0'; // clearing buffer
  }

}
//----------------------------------------------------------------------------

void createJSON()
{  
//  strcpy(msg, str); 
  srtJSON(str);                                 //Start JSON string
  addJSON(str,"rP",  random(500, 6000));    //Add a variable
//  addJSON(str,"aP",  emon.apparentPower);    //Add a variable
  addJSON(str,"pF",  random(0.56, 1.0));     //Add a variable
  addJSON(str,"V",  240);     //Add a variable
  addJSON(str,"I",  random(1.0, 10.0));     //Add a variable
  endJSON(str);                                 //End JSON string
}

