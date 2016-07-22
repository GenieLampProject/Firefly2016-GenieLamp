/*
  Serial Call and Response
 Language: Wiring/Arduino

 This program sends an ASCII A (byte of value 65) on startup
 and repeats that until it gets some data in.
 Then it waits for a byte in the serial port, and
 sends three sensor values whenever it gets a byte in.

 Thanks to Greg Shakar and Scott Fitzgerald for the improvements

   The circuit:
 * potentiometers attached to analog inputs 0 and 1
 * pushbutton attached to digital I/O 2

 Created 26 Sept. 2005
 by Tom Igoe
 modified 24 April 2012
 by Tom Igoe and Scott Fitzgerald

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/SerialCallResponse

 */
#include <DmxMaster.h>
int firstSensor = 0;    // first analog sensor
int secondSensor = 0;   // second analog sensor
int thirdSensor = 0;    // digital sensor
int inByte;         // incoming serial byte
/**Light Stuff **/
int red=0;
int green=0;
int blue=0;
/**Smoke Stuff **/
int smoke = 0;
void setup() {

 /**DMX MASTER CODE**/
   /* The most common pin for DMX output is pin 3, which DmxMaster
** uses by default. If you need to change that, do it here. */
  DmxMaster.usePin(3);

  /* DMX devices typically need to receive a complete set of channels
** even if you only need to adjust the first channel. You can
** easily change the number of channels sent here. If you don't
** do this, DmxMaster will set the maximum channel number to the
** highest channel you DmxMaster.write() to. */
  DmxMaster.maxChannel(512);
 /**COMMUNICATIONS CODE**/
  // start serial port at 9600 bps:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  establishContact();  // send a byte to establish contact until receiver responds
}

void loop() {
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 0) {
    // get incoming byte:
    int smokeIn = Serial.parseInt();
    int redIn = Serial.parseInt();
    int greenIn = Serial.parseInt();
    int blueIn = Serial.parseInt();
     if (Serial.read() == '\n') {
    if (redIn && blueIn && greenIn == 0){
      red= red--;
      blue = blue--;
      green = green--;
      smoke = smokeIn;
    }
    else{
      red = redIn;
      blue = blueIn;
      green = greenIn;
      smoke = smokeIn;
    }
    DmxMaster.write(2, red);
    Serial.print("Red Brightness: ");
    Serial.println(red);
    DmxMaster.write(3, blue);
    Serial.print("Blue Brightness: ");
    Serial.println(blue);
    DmxMaster.write(4, green);
    Serial.print("Green Brightness: ");
    Serial.println(green);
    if (smoke != 0){
    DmxMaster.write(1, 255);
    Serial.println("machine On poofing");
    delay(smoke*1000);
    DmxMaster.write(1, 0);
    Serial.println("machine stopped poofing");
    }
  }
  }
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.print('A');   // send a capital A
    delay(300);
  }
}
