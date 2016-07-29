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
// uncomment the following to disable serial debug statements
#define SERIAL_DEBUG false
#include <SerialDebug.h>
#include <DmxMaster.h>
#define RED_DEFAULT 255
#define GREEN_DEFAULT 69
#define BLUE_DEFAULT 0
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
      pinMode(inputPin,INPUT);
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
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

 // establishContact();  // send a byte to establish contact until receiver responds
}

void loop() {
  // if we get a valid byte, read analog ins:
  buttonPressed = checkButton();
 DEBUG(buttonPressed);
  if (!buttonPressed){
  if (Serial.available() > 0) {
    // get incoming byte:
    int smokeIn = Serial.parseInt();
    int redIn = Serial.parseInt();
    int greenIn = Serial.parseInt();
    int blueIn = Serial.parseInt();
     if (Serial.read() == '\n') {
      digitalWrite (ledPin, HIGH);
      red = redIn;
      blue = blueIn;
      green = greenIn;
      smoke = smokeIn;
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
    DmxMaster.write(1, 0);
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
    else{
      digitalWrite (ledPin, LOW);
    }
  }
 // DEBUG(smokeON);
  }
  else{
    DEBUG("smoke intensity: ",smoke,"Red Brightness: ",RED_DEFAULT,"Blue Brightness: ",BLUE_DEFAULT,"Green Brightness: ",GREEN_DEFAULT);
    DmxMaster.write(2, RED_DEFAULT);
    DmxMaster.write(3, BLUE_DEFAULT);
    DmxMaster.write(4, GREEN_DEFAULT);
  }
}
/*
void establishContact() {
  while (Serial.available() <= 0) {
    Serial.print('A');   // send a capital A
    delay(300);
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
    return true;
 }
 else if(teensyPin == HIGH){
     return true; 
 }
 else{
      DEBUG("smoke intensity: ",smoke,"Red Brightness: ",RED_DEFAULT,"Blue Brightness: ",BLUE_DEFAULT,"Green Brightness: ",GREEN_DEFAULT);
    DmxMaster.write(2, 0);
    DmxMaster.write(3, 0);
    DmxMaster.write(4, 0);
    DmxMaster.write(1, 0);
    DEBUG("machine stopped poofing");
  //  digitalWrite (ledPin, LOW);
  return false;
 }
  }
