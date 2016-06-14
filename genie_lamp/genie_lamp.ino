/*** control.ino
 *      Master control for the genie lamp project
 ***/


/*** BEGIN Constants ***/
// XXX TODO XXX Add constants not folded into subsystem classes
/*** FINISH Constants ***/


/*** BEGIN Config ***/
// XXX TODO XXX Add steps for what happens to each output when the lamp has been rubbed for a given amount of time
/*** FINISH Config ***/




/*** BEGIN Setup Routine ***/
void setup() {
    TouchSense.setup();
    Poofer.setup();
    BodyLEDs.setup();
}
/*** FINISH Setup Routine ***/




/*** BEGIN Main operation entry point ***/
void loop() {
    // Initialize
    TouchSense.initialize();
    Poofer.initialize();
    BodyLEDs.initialize();
    

    // Main operational loop
    int touched_seconds;
    while (true) {
        touched_millis = TouchSense.touched_time();
        if (!touched_millis) {
            Poofer.off();
            BodyLEDs.off();
        } else {
            // XXX TODO XXX See if we should progress to the next output stage
            Poofer.display(!touched_millis) {
            BodyLEDs.display();
        }
    }

}
/*** FINISH Main operation entry point ***/


/*
const int pins = 2;
int touchPin[pins] = {0, 1};
boolean touched = false;
//int potPin = A0;
int ledPin = 13; //LED conectado ao pino digital 13
int sensitivity = 400;
int spark = 2;
int read1[pins], base[pins];
int column, row, c;
void setup() {
   Serial.begin(38400);
   for (int c=0; c<pins; c++) {
     base[c]=touchRead(touchPin[c]);// Baseline calibration
      Serial.println(base[c]);
pinMode (ledPin, OUTPUT);
pinMode (spark, OUTPUT);
   }
}

void loop() {
// sensitivity =  map(analogRead(potPin), 1, 1023, 300, 1000);
 //Serial.println(sensitivity);
 //Serial.println(analogRead(potPin));
  // put your main code here, to run repeatedly:
digitalWrite(spark, LOW);
digitalWrite (ledPin, LOW);
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
         Serial.print(" Sensor: ");
         Serial.print(sensorNum);
         Serial.print(" Base: "); // print a space
         Serial.print(base[c]);
         Serial.print("  Read: "); // print a space
         Serial.print(read1[c]);
         Serial.print(" BL diff: "); // print a space
         Serial.print(Difference);
         Serial.print(" Threshold: "); // print a space
         Serial.print(sensitivity);
         Serial.println();
       digitalWrite(spark, HIGH);
       digitalWrite (ledPin, HIGH);
       delay(400);
       }
  }
if (touched) {

   // Serial.println(read1[c]-base[c]);
}
}
*/
