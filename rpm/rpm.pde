volatile byte rpmcount;

 unsigned int rpm;

 unsigned long timeold;

 void setup()
 {
   Serial.begin(9600);
   digitalWrite(2, HIGH);
   attachInterrupt(0, rpm_fun, RISING);

   rpmcount = 0;
   rpm = 0;
   timeold = 0;
 }

 void loop()
 {
   if (rpmcount >= 20) {
     //Update RPM every 20 counts, increase this for better RPM resolution,
     //decrease for faster update
     rpm = (30*1000/(millis() - timeold)*rpmcount)*0.25434/60;
     timeold = millis();
     rpmcount = 0;
     Serial.println(rpm,DEC);
   }
 }

 void rpm_fun()
 {
   rpmcount++;
   //Each rotation, this interrupt function is run twice
 }
