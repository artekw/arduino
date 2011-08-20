#include <RF12.h>
#include <Ports.h>
#include <weather_data.h>

void setup () {
    Serial.begin(57600);
    Serial.println("\n[WeatherStation]");
    rf12_initialize('R', RF12_433MHZ, 212);
}

void loop () {
    if (rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof pomiar) {
        memcpy(&pomiar, (byte*) rf12_data, sizeof pomiar);
        Serial.print("WS ");
        Serial.print("L");
        Serial.print(pomiar.light, DEC);
        Serial.print(" H");
        Serial.print(pomiar.humi);
        Serial.print(" T");
        Serial.print(pomiar.temp);
        Serial.print(" P");
        Serial.print(pomiar.pressure);
        Serial.print(" S");
//        Serial.print(pomiar.wind);
//        Serial.print(" W" );
        Serial.print(pomiar.lobat, DEC);
        Serial.print(" B");
        Serial.print(pomiar.battvol);
//        Serial.print(' ');
        Serial.println();

    }
}
