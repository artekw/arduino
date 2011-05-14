char end =0; //latest number recieved from script

//Pin connected to Pin 12 of 74HC595 (Latch)
int latchPin = 2;
//Pin connected to Pin 11 of 74HC595 (Clock)
int clockPin = 3;
//Pin connected to Pin 14 of 74HC595 (Data)
int dataPin = 4;
uint8_t led[6];
void setup() {
//set pins to output
pinMode(latchPin, OUTPUT);
pinMode(clockPin, OUTPUT);
pinMode(dataPin, OUTPUT);

//set display to zero
led[0] = B11111;
led[1] = B11111;
led[2] = B00000;
led[3] = B00000;
led[4] = B00000;
led[5] = B00000;
led[6] = B00000;
Serial.begin(9600);
}


void loop() {

led[0] = B10111;
 
delay(3000); //delay before updating
Serial.print("loop\n");
}

void screenUpdate() {
uint8_t row = B00001;
for (byte k = 0; k < 6; k++) {
// Open up the latch ready to receive data
digitalWrite(latchPin, LOW);
shiftIt(~row );
shiftIt(led[k] ); // LED array
// Close the latch, sending the data in the registers out to the
//matrix
digitalWrite(latchPin, HIGH); row = row << 1;
}
}


void shiftIt(byte dataOut) {
// Shift out 8 bits LSB first,
// on rising edge of clock
boolean pinState;
//clear shift register read for sending data
digitalWrite(dataPin, LOW);
// for each bit in dataOut send out a bit
for (int i=0; i<7; i++) {
//set clockPin to LOW prior to sending bit
digitalWrite(clockPin, LOW);
// if the value of DataOut and (logical AND) a bitmask
// are true, set pinState to 1 (HIGH)
if ( dataOut & (1<<i) ) {
pinState = HIGH;
}
else {
pinState = LOW;
}
//sets dataPin to HIGH or LOW depending on pinState
digitalWrite(dataPin, pinState);
//send bit out on rising edge of clock
digitalWrite(clockPin, HIGH);
digitalWrite(dataPin, LOW);
}
//stop shifting
digitalWrite(clockPin, LOW);
}


