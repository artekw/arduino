#include <EtherShield.h>
#include <stdlib.h>
#include <string.h>

static uint8_t mymac[6] = { 0x54,0x55,0x58,0x10,0x00,0x25 };

static uint8_t myip[4] = { 0,0,0,0 };
static uint8_t mynetmask[4] = { 0,0,0,0 };

// IP address of the host being queried to contact (IP of the first portion of the URL):
static uint8_t websrvip[4] = { 0, 0, 0, 0 };

// Default gateway. The ip address of your DSL router. It can be set to the same as
// websrvip the case where there is no default GW to access the 
// web server (=web server is on the same lan as this host) 
static uint8_t gwip[4] = { 0,0,0,0};

static uint8_t dnsip[4] = { 0,0,0,0 };
static uint8_t dhcpsvrip[4] = { 0,0,0,0 };

#define DHCPLED 13

// listen port for tcp/www:
#define MYWWWPORT 80

#define BUFFER_SIZE 750
static uint8_t buf[BUFFER_SIZE+1];

char numstr[6];

EtherShield es=EtherShield();

// Programmable delay for flashing LED
uint16_t delayRate = 0;

void setup() {

  Serial.begin(19200);
  Serial.println("DHCP Client test");
  pinMode( DHCPLED, OUTPUT);
  digitalWrite( DHCPLED, HIGH);    // Turn it off

  for( int i=0; i<6; i++ ) {
    Serial.print( mymac[i], HEX );
    Serial.print( i < 5 ? ":" : "" );
  }
  Serial.println();
  
  // Initialise SPI interface
  es.ES_enc28j60SpiInit();

  // initialize enc28j60
  Serial.println("Init ENC28J60");
  es.ES_enc28j60Init(mymac);

  Serial.println("Init done");
  
  Serial.print( "ENC28J60 version " );
  Serial.println( es.ES_enc28j60Revision(), HEX);
  if( es.ES_enc28j60Revision() <= 0 ) {
    Serial.println( "Failed to access ENC28J60");

    while(1);    // Just loop here
  }
  
  Serial.println("Requesting IP Addresse");
  // Get IP Address details
  if( es.allocateIPAddress(buf, BUFFER_SIZE, mymac, 80, myip, mynetmask, gwip, dhcpsvrip, dnsip ) > 0 ) {
    // Display the results:
    Serial.print( "My IP: " );
    printIP( myip );
    Serial.println();

    Serial.print( "Netmask: " );
    printIP( mynetmask );
    Serial.println();

    Serial.print( "DNS IP: " );
    printIP( dnsip );
    Serial.println();

    Serial.print( "GW IP: " );
    printIP( gwip );
    Serial.println();
   
    digitalWrite( DHCPLED, HIGH);
    delayRate = 1000;
  } else {
    Serial.println("Failed to get IP address");
    delayRate = 200;
  }

    es.ES_init_ip_arp_udp_tcp(mymac,myip, MYWWWPORT);
}

// Output a ip address from buffer
void printIP( uint8_t *buf ) {
  for( int i = 0; i < 4; i++ ) {
    Serial.print( buf[i], DEC );
    if( i<3 )
      Serial.print( "." );
  }
}

void loop(){
  uint16_t plen, dat_p;
while(1) {
    // read packet, handle ping and wait for a tcp packet:
    dat_p=es.ES_packetloop_icmp_tcp(buf,es.ES_enc28j60PacketReceive(BUFFER_SIZE, buf));

    /* dat_p will be unequal to zero if there is a valid 
     * http get */
    if(dat_p==0){
      // no http request
      continue;
    }
    // tcp port 80 begin
    if (strncmp("GET ",(char *)&(buf[dat_p]),4)!=0){
      // head, post and other methods:
      dat_p=http200ok();
      dat_p=es.ES_fill_tcp_data_p(buf,dat_p,PSTR("<h1>200 OK</h1>"));
      goto SENDTCP;
    }
    // just one web page in the "root directory" of the web server
    if (strncmp("/ ",(char *)&(buf[dat_p+4]),2)==0){
      dat_p=print_webpage(buf);
      goto SENDTCP;
    }
    else{
      dat_p=es.ES_fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 401 Unauthorized\r\nContent-Type: text/html\r\n\r\n<h1>401 Unauthorized</h1>"));
      goto SENDTCP;
    }
SENDTCP:
    es.ES_www_server_reply(buf,dat_p); // send web page data
    // tcp port 80 end
    
}
}

uint16_t print_webpage(uint8_t *buf)
{
  uint16_t plen;
  plen=http200ok();
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("<html><head><title>Light@NANODE</title></head>"));
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("<meta http-equiv=refresh content=5 ><body>"));
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("A0: "));
  itoa(1024-analogRead(0),numstr,10);
  plen=es.ES_fill_tcp_data(buf,plen,numstr);
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("<br><br>Autorefresh evry 5s."));
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("<br>") );


  return(plen);
}

uint16_t http200ok(void)
{
  return(es.ES_fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n")));
}



