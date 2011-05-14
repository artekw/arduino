//-----------------------------------------------------------------------------------------------
// JSON functions
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// JSON structure characters
//-----------------------------------------------------------------------------------------------
                                                      //JSON structure uses " = 34
char jsonS[] = {'{',34,'\0'};                         //JSON start characters
char jsonB1[] = {34,':',34,'\0'};                     //JSON characters between key and value
char jsonB2[] = {34,',',34,'\0'};                     //JSON characters between each data pair
char jsonE[] = {34,'}','\0'};                         //JSON end characters

//-----------------------------------------------------------------------------------------------
// Adds start of JSON characters
//-----------------------------------------------------------------------------------------------
char * srtJSON ( char * destination )
{
    strcat(destination,jsonS);  
}

//-----------------------------------------------------------------------------------------------
// Adds a JSON key and data pair to the string
//-----------------------------------------------------------------------------------------------
char * addJSON ( char * destination, const char * source, const double value )
{
  strcat(destination,source);
  strcat(destination,jsonB1);
  strcat(destination,doubleString(value,2));
  strcat(destination,jsonB2);
}

//-----------------------------------------------------------------------------------------------
// Adds end of JSON characters
//-----------------------------------------------------------------------------------------------
char * endJSON ( char * destination )
{
    str[strlen(destination)-3]='\0';
    strcat(destination,jsonE);  
}

//-----------------------------------------------------------------------------------------------
// Converts a double in to a string
//-----------------------------------------------------------------------------------------------
char * doubleString(double value, int precision )
{
   char intPartStr[20];
   char decPartStr[20];
   char doubleStr[20] = "";
   
   if (value<0) 
   {
     value=value*-1;
     strcat(doubleStr, "-");     
   }
   
   int intPart = (int)value;
      
   double decPart = (value-intPart)*(pow(10,precision));

   itoa(intPart, intPartStr, 10);
   itoa((int)decPart, decPartStr, 10);
   
   strcat(doubleStr, intPartStr);   
   strcat(doubleStr, ".");
   strcat(doubleStr, decPartStr);
   return doubleStr;
}
