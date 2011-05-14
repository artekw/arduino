//-----------------------------------------------------------------------------------------------
// Ethernet functions
//-----------------------------------------------------------------------------------------------

#define BUFFER_SIZE 500
static uint8_t buf[BUFFER_SIZE+1];

EtherShield es=EtherShield();

uint16_t dat_p;
uint16_t replyPosition;

int callbackRecieved=0;

int maxMsRetry=1000;
int maxRetry = 5;

//-----------------------------------------------------------------------------------------------
// Setup ethernet shield
//-----------------------------------------------------------------------------------------------
void client_setup(byte* mac,byte* ip,byte* gateway, byte* server)
{
  es.ES_enc28j60Init(mac);
  es.ES_init_ip_arp_udp_tcp(mac, ip, port);
  es.ES_client_set_gwip(gateway);
  es.ES_client_set_wwwip(server);
}

//-----------------------------------------------------------------------------------------------
// Set timeout and retry variables
//-----------------------------------------------------------------------------------------------
void client_timeout(int _maxRetry, int _maxMsRetry)
{
  maxRetry = _maxRetry;
  maxMsRetry = _maxMsRetry;
}

//-----------------------------------------------------------------------------------------------
// ethernet_send handles the process of sending data, it also waits for a reply from the server
// to verify that the send was sucessful.
//-----------------------------------------------------------------------------------------------
int ethernet_send(char * domainname,char * string)
{
  //-----------------------------
  // state 0 : data not sent 
  // state 1 : data sent
  //----------------------------
  int state = 0;
  int timeoutCount = 0;                 //Counter for counting number of failed attempts
  unsigned long time;                   //hold time data was sent
  
  while (state<2)                       //Loop until state reaches 2 or exit
  {
    
    //----------------------------------------------------------------------------------
    // 1) Send data
    //----------------------------------------------------------------------------------
    if (client_ready() && state==0)   
    {
      callbackRecieved = 0;
      
      //THE ACTUAL SEND COMMAND------------------------------------------------------
      es.ES_client_browse_url(PSTR(""), string, domainname, &browserresult_callback);
      
      time = millis();                  //Reset timeout counter
      state = 1;
    }
    
    //----------------------------------------------------------------------------------
    // 2) If reply recieved - return success!
    //----------------------------------------------------------------------------------    
    if (client_replyAvailable() && state==1)        //If reply recieved
    {
      return 1;                                     //end function with 1 for success!
    }
    
    //----------------------------------------------------------------------------------
    //If no reply is recieved : retry 5 times
    //----------------------------------------------------------------------------------
    if (state == 1 && (millis()-time)>maxMsRetry) {state = 0; timeoutCount++;}
    if (timeoutCount>maxRetry) return 0;      //end function with a 0 for failed to send.
    
  }
}
//--------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Client ready?
//-----------------------------------------------------------------------------------------------
int client_ready()
{
  // handle ping and wait for a tcp packet - calling this routine powers the sending and receiving of data
  dat_p=es.ES_packetloop_icmp_tcp(buf,es.ES_enc28j60PacketReceive(BUFFER_SIZE, buf));
  if (dat_p == 0) return 1;
  return 0;
}

//-----------------------------------------------------------------------------------------------
// Callback function
//-----------------------------------------------------------------------------------------------
void browserresult_callback(uint8_t statuscode,uint16_t datapos)
{ 
  replyPosition = datapos;
  client_printData();
}

//-----------------------------------------------------------------------------------------------
// Reply available
//-----------------------------------------------------------------------------------------------
int client_replyAvailable()
{
  return callbackRecieved;
}

//-----------------------------------------------------------------------------------------------
// Print all of the reply
//-----------------------------------------------------------------------------------------------
void client_printReply()
{
  if (replyPosition!=0)
  {
   int pos = replyPosition;
   while(buf[pos])
   {
     Serial.print((char)buf[pos]);
     pos++;
   } 
   Serial.println();
  }
  
}

//-----------------------------------------------------------------------------------------------
// Fetch the data bit from the reply
//-----------------------------------------------------------------------------------------------
void client_printData()
{
  if (replyPosition!=0)
  {
   int pos = replyPosition;
   int startpos;
   
   while(buf[pos])
   {
     if ((buf[pos-1] == '\n') && (buf[pos] == '\r')) {startpos = pos+2;}
     Serial.print(buf[pos]);
     pos++;
     
   } 
   //Create string char array, +1 for null string terminator
   char dstr[34];
   //Transfer characters from buf array to our string char array
   int i;
   for (i=0; i<34; i++)
   {
     pos = startpos + i;
     dstr[i] = (char) buf[pos];
   }
   //Terminate string with null
   dstr[i+1] = '\0';
   if (dstr[0] == 'o' && dstr[1] == 'k') 
   {
     callbackRecieved=1;
     
   }
   else
   {
     Serial.println(dstr);
     
   }
   
  }
  
}

