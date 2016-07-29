#include <SoftwareSerial.h>

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

 Created 26 Sept. 2005z
 by Tom Igoe
 modified 24 April 2012
 by Tom Igoe and Scott Fitzgerald

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/SerialCallResponse

 */
 /**BEGIN Debug toggles**/
// uncomment the following to disable serial debug statements
//#define SERIAL_DEBUG false
#define SOFTWARE_SERIAL
/**END Debug toggles**/

/**BEGIN Includes**/
#include <SerialDebug.h>
#include <DmxMaster.h>
/**END Includes**/
 
/**BEGIN Testing Macros**/
#ifdef SOFTWARE_SERIAL
#define SWSERIAL_DECLARE SoftwareSerial mySerial(10, 11)
#define SWSERIAL_BEGIN(x) mySerial.begin(x)
#define HWSERIAL_BEGIN(x)
#define SERIAL_DEBUG true
#define SERIAL_CHOICE mySerial
#else
#define SWSERIAL_DECLARE
#define SWSERIAL_BEGIN(x)
#define HWSERIAL_BEGIN(x) Serial.begin(x)
#define SERIAL_CHOICE Serial
#define SERIAL_DEBUG false
#endif
/**END Testing Macros**/



#define RED_DEFAULT 255
#define GREEN_DEFAULT 69
#define BLUE_DEFAULT 0
 // RX, TX
SWSERIAL_DECLARE;
//button stuff//
const int buttonPin = 2;   
const int inputPin = 4;  
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
/**Light Stuff **/
int red=0;
int green=0;
int blue=0;
/**Smoke Stuff **/
int smoke = 0;
boolean smokeON = false;
int ledPin = 13;
long smokeTime = 0;
boolean buttonPressed = false;
void setup() {
      pinMode(buttonPin,INPUT_PULLUP);
pinMode (ledPin, OUTPUT);
      pinMode(inputPin,INPUT_PULLUP);
digitalWrite (ledPin, LOW);
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
  SERIAL_DEBUG_SETUP(38400);
  SWSERIAL_BEGIN(19200);
  HWSERIAL_BEGIN(19200);
  SERIAL_CHOICE.setTimeout(100);

 // establishContact();  // send a byte to establish contact until receiver responds
}

void loop() {
  // if we get a valid byte, read analog ins:
  buttonPressed = checkButton();
 DEBUG("buttonPressed",buttonPressed);
  if (buttonPressed == 0){
  if (SERIAL_CHOICE.available() > 0) {
    DEBUG("Serial Available",SERIAL_CHOICE.available());
    // get incoming byte:
    int smokeIn = SERIAL_CHOICE.parseInt();
    DEBUG("parse smoke");
    int redIn = SERIAL_CHOICE.parseInt();
        DEBUG("parse red");
    int greenIn = SERIAL_CHOICE.parseInt();
            DEBUG("parse green");
    int blueIn = SERIAL_CHOICE.parseInt();
            DEBUG("parse blue");
     if (SERIAL_CHOICE.read() == '\n') {
      digitalWrite (ledPin, HIGH);
      if (redIn == red && blueIn == blue && greenIn == green && smokeIn == smoke){
      bool noChange = true;
      DEBUG("no change");
    }
    else{
      red = redIn;
      blue = blueIn;
      green = greenIn;
      smoke = smokeIn;
      DEBUG("change detected ");
    DEBUG("smoke intensity: ",smoke,"Red Brightness: ",red,"Blue Brightness: ",blue,"Green Brightness: ",green);
    DmxMaster.write(2, red);
    DmxMaster.write(3, blue);
    DmxMaster.write(4, green);
    if (smokeON == true){
      if(smoke > 0){
    DmxMaster.write(1, 255);
    DEBUG("machine still poofing",smokeON);
   // digitalWrite (ledPin, HIGH);
      }
      else{
      red = red-1;
      blue = blue-1;
      green = green-1;
      smoke = smoke-1;
    DmxMaster.write(1, 0);
    DmxMaster.write(2, 0);
    DmxMaster.write(3, 0);
    DmxMaster.write(3, 0);
    DEBUG("machine stopped poofing",smokeON);
  //  digitalWrite (ledPin, LOW);
    smokeON = false;
      }
    }
   else {
      if (smoke > 0){
        DmxMaster.write(1, smoke);
    DEBUG("machine On poofing", smokeON);
  //  digitalWrite (ledPin, HIGH);
    smokeON = true;
      }
    }
    }
    }// this
    else{
      digitalWrite (ledPin, LOW);
    }
  }
  else {
    DEBUG("serial not available");
  }
 // DEBUG(smokeON);
  }
  else{
    DEBUG("button was pressed");
    DEBUG("smoke intensity: ",smoke,"Red Brightness: ",RED_DEFAULT,"Blue Brightness: ",BLUE_DEFAULT,"Green Brightness: ",GREEN_DEFAULT);
    DmxMaster.write(2, RED_DEFAULT);
    DmxMaster.write(3, BLUE_DEFAULT);
    DmxMaster.write(4, GREEN_DEFAULT);
  }
}
/*
void establishContact() {
  while (SERIAL_CHOICE.available() <= 0) {
    SERIAL_CHOICE.print('A');   // send a capital A
    delay(300);
  }
}
*/


/*//serial event def
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
*/
bool checkButton() {
int reading = digitalRead(buttonPin);
int teensyPin = digitalRead(inputPin);
//
//  // check to see if you just pressed the button 
//  // (i.e. the input went from LOW to HIGH),  and you've waited 
//  // long enough since the last press to ignore any noise:  
//
//  // If the switch changed, due to noise or pressing:
//  if (reading != lastButtonState) {
//    // reset the debouncing timer
//    lastDebounceTime = millis();
//  } 
//  
//  if ((millis() - lastDebounceTime) > debounceDelay) {
//    // whatever the reading is at, it's been there for longer
//    // than the debounce delay, so take it as the actual current state:
//    buttonState = reading;
//    if (buttonState == HIGH){
//          DmxMaster.write(2, 0);
//    DEBUG("Red Brightness: ");
//    DEBUG(0);
//    DmxMaster.write(3, 0);
//    DEBUG("Blue Brightness: ");
//    DEBUGln(0);
//    DmxMaster.write(4, 0);
//    DEBUG("Green Brightness: ");
//    DEBUGln(0);
//    DmxMaster.write(1, 0);
//    DEBUGln("machine stopped poofing");
//  //  digitalWrite (ledPin, LOW);
//    }
//  }
//  
//  // set the LED using the state of the button:
//
// 
//
//  // save the reading.  Next time through the loop,
//  // it'll be the lastButtonState:
//  lastButtonState = reading;
   if(reading == LOW){
    DEBUG("button active");
    return true;
 }
 else if(teensyPin == LOW){
  DEBUG("teensy pin LOW");
     return true; 
 }
 else{
    DEBUG("button not active");
    DEBUG("teensyPin",teensyPin,"smoke intensity: ",0,"Red Brightness: ",0,"Blue Brightness: ",0,"Green Brightness: ",0);
    DmxMaster.write(2, 0);
    DmxMaster.write(3, 0);
    DmxMaster.write(4, 0);
    DmxMaster.write(1, 0);
    DEBUG("machine stopped poofing");
  //  digitalWrite (ledPin, LOW);
  return false;
 }
  }
