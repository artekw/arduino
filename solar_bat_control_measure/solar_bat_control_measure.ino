
int mbatpin = 1;
int msolpin = 2;
int ldrpin = 0;
int mosfetpin = 5;

int batvol = 0;
int solvol = 0;
float vout = 0.0;

void setup() 
{
  analogReference(INTERNAL); 
  Serial.begin(9600);
  mControl(mosfetpin, 0);
}

void  loop() 
{
  mControl(mosfetpin, 1);
  int batval = analogRead(mbatpin);
  int solval = analogRead(msolpin);
//  int ldr = analogRead(ldrpin);
//  ldr = 1023 - ldr;
//  int ldrval = map((ldr), 0, 1023, 0, 100);
  
  // 0 - bateria nie jest ladowana z panela slonecznego
  //     wowczas mozna wykonac niezalezny pomiar napiecia panela
  //     Napiecie nie "wychodzi" na zewnatrz  
  // 1 - bateria jest ladowana z panela slonecznego
  //     Napięcie z panela slonecznego wychodzi na zewnatrz
  //     mozna zasilac lub ladować baterie
  
  
  Serial.print("BatVol: ");
  Serial.print(batval/1024) * 1.1 * 2;
  Serial.println(' ');
  
  Serial.print("SolVol: ");
  Serial.print(map(solval, 0, 4096, 0, 660));
  Serial.println(' ');
  Serial.println("----------------");
  
  
  delay(2000);
}

static void mControl(byte fet, byte on) {
  pinMode(fet, OUTPUT);
  digitalWrite(fet, on);
  delay(100);
}
