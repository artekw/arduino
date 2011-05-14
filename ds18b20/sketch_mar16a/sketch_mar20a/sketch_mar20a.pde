// Matrix

int randRow[7];
int randCol[4];

int row[] = {16,15,14,13,12,11,10};
int col[] = {6,5,4,3,2};

void setup() {
  for (int thisPin = 0; thisPin < 7; thisPin++) {
    // initialize the output pins for matrix 1:
    pinMode(row[thisPin], OUTPUT);
    pinMode(col[thisPin], OUTPUT); 
    digitalWrite(col[thisPin], HIGH);   
    randomSeed(analogRead(5));

 
  }
}
void loop() {

//  randRow[7] = random(10,16);
//  randCol[4] = random(2,6);
  
  // light up the first matrix:
  // iterate over the rows (anodes):
  for (int thisrow = 0; thisrow < 7; thisrow++) {
    // take the row pin (anode) high:
    digitalWrite(row[thisrow], HIGH);
    // iterate over the cols (cathodes):
    for (int thiscol = 0; thiscol < 5; thiscol++) {
      // when the row is high and the col is low,
      // the LED where they meet turns on:
      digitalWrite(col[thiscol], LOW);
      delay(100);
      // take the col pin (cathode) high to turn the LED off:
      digitalWrite(col[thiscol], HIGH);      
    }
    // take the row pin low to turn off the whole row:
    digitalWrite(row[thisrow], LOW);
  }

}
