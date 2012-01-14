
int mbatpin = 3;
int msolpin = 2;
int mosfetpin = 4;

int batvol = 1;
int solvol = 0;

void setup() 
{
  Serial.begin(9600);
}

void  loop() 
{
  int batval = analogRead(batvol);
  int solval = analogRead(solvol);
  
  // 0 - bateria nie jest ladowana z panela slonecznego
  //     wowczas mozna wykonac niezalezny pomiar napiecia panela
  //     Napiecie nie "wychodzi" na zewnatrz  
  // 1 - bateria jest ladowana z panela slonecznego
  //     Napięcie z panela slonecznego wychodzi na zewnatrz
  //     mozna zasilac lub ladować baterie
//  mControl(mbatpin, 1);
//  mControl(msolpin, 1);
  mControl(mosfetpin, 0);
  
  Serial.print("BatVol: ");
  Serial.print(batval);
  Serial.println(' ');
  
  Serial.print("SolVol: ");
  Serial.print(solval);
  Serial.println(' ');
  Serial.println("----------------");
  
  
  delay(2000);
}

static void mControl(byte fet, byte on) {
  pinMode(fet, OUTPUT);
  digitalWrite(fet, !on);
  delay(100);
}
