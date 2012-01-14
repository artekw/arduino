#include <JeeLib.h>

typedef struct {
	int lp;
	float temperatura;
} Payload;
Payload pomiar;

void setup () {
    Serial.begin(57600);
    Serial.println("\n[Sensnode Demo]");
    rf12_initialize('R', RF12_433MHZ, 212);
}

void loop () {
    if (rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof pomiar) {
        memcpy(&pomiar, (byte*) rf12_data, sizeof pomiar);
        Serial.print("WS ");
        Serial.print("LP");
        Serial.print(pomiar.lp);
        Serial.print(" T");
        Serial.print(pomiar.temperatura);
        Serial.println();
    }
}
