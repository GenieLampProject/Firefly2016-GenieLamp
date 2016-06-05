const int pins = 2;
int touchPin[pins] = {0, 1};
boolean touched = false;
int sensitivity = 400;
int read1[pins], base[pins];
int column, row, c;
void setup() {
   Serial.begin(38400);
   for (int c=0; c<pins; c++) {
     base[c]=touchRead(touchPin[c]); // Baseline calibration
   }
}

void loop() {
  // put your main code here, to run repeatedly:
touched = false;
  for (c=0; c<pins; c++) {
     read1[c] = touchRead(touchPin[c]);
     //Serial.println(read1[c]-base[c]);
     int Difference = read1[c]-base[c];
       if (read1[c]-base[c] > sensitivity) {
         touched = true;
         String sensorNum = c;
        // Disabled for Open Studios
        //String output = " Sensor " + sensorNum;
        // Serial.println(output);
        // Serial.println(Difference);
                 Serial.print(sensorNum);
         Serial.print(" "); // print a space
         Serial.print(Difference);
         Serial.print(" "); // print a space
         Serial.print(read1[c]);
         Serial.println();
       }
  }
if (touched) {

   // Serial.println(read1[c]-base[c]);
}
}
