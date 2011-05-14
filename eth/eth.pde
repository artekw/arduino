#include "EtherShield.h"

#define port 80

char str[250];

byte mac[] = {0x54,0x52,0x58,0x10,0x00,0x18};         //Ethernet shield mac address
byte ip[] = {192,168,88,88};                            //Ethernet shield ip address
byte gateway[] = {192,168,88,1};                       //Gateway ip
byte server[] = { 209, 85, 149, 104 }; // Google                   //Server ip

void setup() {
  client_setup(mac,ip,gateway,server);    //Setup ethernet client
  client_timeout(5,3000);                 //Set timeout variables
  Serial.begin(9600);
}

void loop() {
 //strcpy(str,"/post.php?json=");
  if (ethernet_send("","/search?q=arduino"))                     //Try to send the string NOTE: success on recieve of "ok" from server
  { 
    Serial.println("Data sent");
  } 
  else
  { 
    Serial.println("Failed to send"); 
  }
}
