// uncomment the following to disable serial debug statements
//#define SERIAL_DEBUG false
/**Uncomment below line to turn off Communication messages. **/
//#define COMM false
#define SMOKE_SERIAL Serial2
#include <SerialDebug.h>
#include "Timer.h"
#include "FastLED.h"
/*
* Utility functions to help debugging running code.
*/



#ifdef COMM
#define COMM_PRINT(x)  Serial.print (x)
#define COMM_PRINTLN(x)  Serial.println (x)
#else
#define COMM_PRINT(x)
#define COMM_PRINTLN(x)
#endif



/*** control.ino
        Master control for the genie lamp project
 ***/


//debug print line
/*#ifdef DEBUG
  #define DEBUG_SERIAL(x)  Serial.begin(x)
  #define DEBUG_PRINT(x)  Serial.print (x)
  #define DEBUG_PRINTLN(x)  Serial.println (x)
#else
  #define DEBUG_SERIAL(x)
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif
*/

/*** BEGIN Constants ***/
//Genie Lamp Stuff
#define PILOT_PIN 3
//RED
#define POOFER_PIN 4
//blue
#define SPARKER_PIN 5
//White LED
/* for original layout NOT FOR FF
 * #define PILOT_PIN 3
#define POOFER_PIN 4
#define SPARKER_PIN 5
*/
#define LED_PIN_LEFT  22
#define LED_PIN_RIGHT  21

/*TOUCH SENSOR*/
#define TOUCH_TOTAL_PINS 6
/*Left Side*/
#define TOUCH_LEFT_FRONT_PIN  16
#define TOUCH_LEFT_MIDDLE_PIN  17
#define TOUCH_LEFT_BACK_PIN  18

/*Right Side*/
#define TOUCH_RIGHT_FRONT_PIN  15
#define TOUCH_RIGHT_MIDDLE_PIN  1
#define TOUCH_RIGHT_BACK_PIN  0 //PLEASE NOTE


//comment out to disable all debug prints.

//Timer stuff
Timer t;


//FastLED stuff
#define LED_TYPE    WS2812
#define COLOR_ORDER RGB
#define LEFT_NUM_LEDS    106
#define RIGHT_NUM_LEDS    106

CRGB left[LEFT_NUM_LEDS];
CRGB right[RIGHT_NUM_LEDS];

#define BRIGHTNESS          50
#define FRAMES_PER_SECOND  120
int FastupdateTask = 0;
uint8_t brightness = 0;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int fade = 0;
int timeTarget = 10000;
int brightCal = 0;
long timeCalled = 0;
long timeCalledStart = 0;
long timeCalledEnd = 0;
long timeOffEnd = 0;
long timeEnd = 0;
int attempts = 0;
int confidence = 0;

// touch Sensor stuff.
/*not sure where to put these to make them in scope, so I added them here to test.*/
/*Needed for baseline and readings*/
const int pins = TOUCH_TOTAL_PINS;
const int touchPin[pins] = {TOUCH_LEFT_FRONT_PIN, TOUCH_LEFT_MIDDLE_PIN, TOUCH_LEFT_BACK_PIN, TOUCH_RIGHT_FRONT_PIN, TOUCH_RIGHT_MIDDLE_PIN, TOUCH_RIGHT_BACK_PIN};
/*Needed for readings*/
const int sensitivity = 200;
//const int sensitivity = 400;
int read1[pins];
long firstRead[pins] = {0};
long minRead = 0;
/*needed for baseline*/
int base[pins];
long lastBase[pins];

//Poofer Stuff
long pooferStartTime = 0;
int sparkerEvent = 0;
int pooferEvent = 0;
int resting = 0;
boolean poofComplete = false;
long touchEnded = 0;
 long myMillis = 0;
 long Mills = 0;
 long OffDiff = 0;
//time that the last touch ended.


//Smoke Stuff
int spoutRed;
int spoutBlue;
int spoutGreen;
int intensity;
/*** FINISH Constants ***/


/*** BEGIN Config ***/
/* when lamp is rubbed for (in millis):*/
#define LED_START_MILLIS 2000 //Begin LEDS
#define PILOT_START_MILLIS 8000  //open pilot solenoid
#define SPARK_START_MILLIS 8250 //start spark ignitor (.5 second duty)
#define SPARKER_OFF_TIME 500
#define SPARKER_ON_TIME 250
#define SHORT_POOF_ON_MILLIS 10000//open poof solenoid
#define SHORT_POOF_OFF_MILLIS 10100//close poof solenoid
#define MED_POOF_ON_MILLIS 10500//open poof solenoid
#define MED_POOF_OFF_MILLIS 10700//close poof solenoid
#define LONG_POOF_ON_MILLIS 10900//open poof solenoid
#define LONG_POOF_OFF_MILLIS 11200//close poof solenoid
#define FINAL_POOF_ON_MILLIS 11400//open poof solenoid
#define FINAL_POOF_OFF_MILLIS 13000//close poof solenoid
 /* when lamp NOT rubbed for: */
#define LED_END_MILLIS 500 //Begin LED Fade
#define LED_OFF_MILLIS 5000 //Begin LED Fade
#define SPARK_END_MILLIS 2000 //stop spark ignitor
#define PILOT_END_MILLIS 3000//stop pilot solenoid
/*** FINISH Config ***/


// XXX TODO XXX Add to the appropriate specific files
/*** BEGIN Modules Interfaces***/
/** BEGIN Modules Declarations ***/
int allsparkEvent = 0;
int singleSparkEvent = 0;
void SparkMaster(){     // XXX JGF XXX TODO XXX Move into Poofer::Spark()
  DEBUG("Called sparkMaster");
  if (sparkerEvent = 0){
allsparkEvent = t.every(SPARKER_OFF_TIME, Spark);
  }
  sparkerEvent = 1;
}

void Spark(){     // XXX JGF XXX TODO XXX Move into Poofer::Spark()
   DEBUG("Called spark");
singleSparkEvent = t.pulse(SPARKER_PIN, SPARKER_ON_TIME, HIGH);
}
int FastUpdateTask = 0;
void FastUpdateSetup(){
  FastUpdateTask = t.every(1000/FRAMES_PER_SECOND, FastUpdate);
}
void FastUpdate(){
  FastLED.show();
  //DEBUG(" showing FASTLED");
}
void Poof(long Duration){     // XXX JGF XXX TODO XXX Move into Poofer::Spark()
  // perform a poof of specified duration
 pooferEvent = t.pulse(POOFER_PIN, Duration, HIGH);
}
class ModuleBase
{
  public:
    virtual void setup() = 0;
    virtual void initialize() = 0;
};

/** BEGIN Input Modules Declarations ***/
class InputBase : public ModuleBase
{
  public:
    virtual long touched_time() = 0;
};

class Touch : public InputBase
{
  public:
    virtual void setup();
    virtual void initialize();
    virtual long touched_time();
  private:
    long _touched_millis = 0;
    long _untouched_millis = 0;
};
/** FINISH Input Modules Declarations ***/

/** BEGIN Output Modules Declarations ***/
class OutputBase : public ModuleBase
{
  public:
    virtual void off(long timeOff) = 0;
    virtual void display(long millis) = 0;
};

class Poofer : public OutputBase
{
  public:
    virtual void setup();
    virtual void initialize();
    virtual void off(long timeOff);
    virtual void display(long millis);
    virtual void Update();
};


class BodyLEDs : public OutputBase
{
  public:
    virtual void setup();
    virtual void initialize();
    virtual void off(long timeOff);
    virtual void display(long millis);
};

class Smoke : public OutputBase
{
  public:
    virtual void setup();
    virtual void initialize();
    virtual void off(long timeOff);
    virtual void display(long millis);
    virtual void Update();
};

/** BEGIN Output Modules Declarations ***/
/** FINISH Modules Declarations ***/

/** BEGIN Modules Definitions ***/
/** BEGIN Touch Modules Definitions ***/
void Touch::setup() {
  SERIAL_DEBUG_SETUP(9600);
    //"touch setup Called");
  //takes a baseline reading for the sensor.
  int bpin;
  for (bpin = 0; bpin < pins; bpin++) {
    base[bpin] = touchRead(touchPin[bpin]); // Baseline calibration
    lastBase[bpin] = millis();
    //DEBUG(base[bpin]);
  }
}
void Touch::initialize() {

}
long Touch::touched_time() {
  //DEBUG("touched_time Called");
  //returns a long from the sensors that tells the duration of the touch
  long timeCalled = millis();
  bool touched = false;
  int tolerance = 300;
  attempts = constrain(attempts, 0, 200);
  confidence = max (confidence, 0);
  int readPin;
  long result;
  long totalTime;
  long minReadResult = 0;
  result = 0;
  long readTime[pins] = {0};
  long timeDiff[pins] = {0};
  for (readPin = 0; readPin < pins; readPin++) {
    String sensorNum = readPin;
    int Difference = read1[readPin] - base[readPin];
   // DEBUG(Difference);
    readTime[readPin] = millis();
    //get the time that the touch was read.
    //calculated the time difference for each read pin.
    read1[readPin] = touchRead(touchPin[readPin]);
    //sample the read pin to detect touch.
     if (Difference > sensitivity) {
      touched = true;
      confidence = confidence+1;
      attempts = tolerance + confidence;
      if (minReadResult == 0){
        minReadResult = readTime[readPin];
     }
     if (firstRead[readPin] == 0){
     firstRead[readPin] = readTime[readPin];
     }
          else if (minReadResult > firstRead[readPin]){
      minReadResult = firstRead[readPin];
     }
    if (timeDiff[readPin] > result) {
    result = timeDiff[readPin];
      }
     }
     else{


     // DEBUG(" PIN EMPTY ");
     }
  }
 // DEBUG(minReadResult);
 //DEBUG(minRead);
  if (touched == true) {
    touchEnded = 0;
    timeCalledEnd = 0;
    if (minReadResult <= minRead){
      minRead = minReadResult;
    }
    totalTime = millis() - minRead;
   // DEBUG(" total time: ", totalTime);
   // DEBUG(" total time: ", minRead);
    return totalTime;
  }
  else if (touched == false){
    if(attempts != 0){
    attempts--;
    confidence = 0;
  //  DEBUG("using attempt", attempts, confidence);
    return totalTime;
  }
  else if(attempts == 0){
    result = 0;
    minReadResult = 0;
    memset(firstRead, 0, sizeof(firstRead));
    confidence = 0;
    minRead = millis();
  //  DEBUG("No Touch");
    if (touchEnded == 0){
      touchEnded = millis();
    }
    return false;
  }
  }
}
/** FINISH Touch Modules Definitions ***/

/** BEGIN Poofer Modules Definitions ***/
void Poofer::setup() {

    pinMode (SPARKER_PIN, OUTPUT);
    digitalWrite (SPARKER_PIN, LOW);
    pinMode (POOFER_PIN, OUTPUT);
    digitalWrite (POOFER_PIN, LOW);
    pinMode (PILOT_PIN, OUTPUT);
    digitalWrite (PILOT_PIN, LOW);
}
void Poofer::initialize() {
  pooferStartTime = 0;
    t.stop(allsparkEvent);
    t.stop(singleSparkEvent);
      digitalWrite (SPARKER_PIN, LOW);
      digitalWrite (POOFER_PIN, LOW);
      digitalWrite (PILOT_PIN, LOW);
    sparkerEvent = 0;
    allsparkEvent = 0;
    singleSparkEvent = 0;

      // XXX TODO XXX
}
void Poofer::Update(){
  // perform a poof of specified duration
 t.update();
}
void Poofer::off(long touchEnded) {
poofComplete = false;
//SPARK_END_MILLIS 2000 //stop spark ignitor
//PILOT_END_MILLIS 3000//stop pilot solenoid
digitalWrite(POOFER_PIN, LOW);
SMOKE_SERIAL.print(0);
COMM_PRINT("printed Poof: ");
COMM_PRINTLN(0);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutRed);
COMM_PRINT("printed Red: ");
COMM_PRINTLN(spoutRed);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutBlue);
COMM_PRINT("printed Blue: ");
COMM_PRINTLN(spoutBlue);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.println(spoutGreen);
COMM_PRINT("printed Green: ");
COMM_PRINTLN(spoutGreen);
t.stop(pooferEvent);
pooferEvent = 0;
OffDiff = millis() - touchEnded;
DEBUG("offDiff",OffDiff, SPARK_END_MILLIS, allsparkEvent, touchEnded,attempts);
if (OffDiff >= SPARK_END_MILLIS && allsparkEvent != 0){
   DEBUG("terminate sparker");
    allsparkEvent = 0;
    t.stop(allsparkEvent);
    t.stop(singleSparkEvent);
    digitalWrite (SPARKER_PIN,LOW);
    sparkerEvent = 0;
    if(OffDiff > PILOT_END_MILLIS){
      digitalWrite (PILOT_PIN, LOW);
    }
}
}
void Poofer::display(long millis) {
 /*
  * when lamp is rubbed for (in millis):
#define LED_START_MILLIS 2000 //Begin LEDS
#define PILOT_START_MILLIS 8000  //open pilot solenoid
#define SPARK_START_MILLIS 8250 //start spark ignitor (.5 second duty)
#define SHORT_POOF_ON_MILLIS 10000//open poof solenoid
#define SHORT_POOF_OFF_MILLIS 10100//close poof solenoid
#define MED_POOF_ON_MILLIS 10500//open poof solenoid
#define MED_POOF_OFF_MILLIS 10700//close poof solenoid
#define LONG_POOF_ON_MILLIS 10900//open poof solenoid
#define LONG_POOF_OFF_MILLIS 11200//close poof solenoid
#define FINAL_POOF_ON_MILLIS 11400//open poof solenoid
#define FINAL_POOF_OFF_MILLIS 13000//close poof solenoid
 /* when lamp NOT rubbed for:
#define LED_END_MILLIS 500 //Begin LED Fade
#define SPARK_END_MILLIS 2000 //stop spark ignitor
#define SPARK_END_MILLIS 3000//stop pilot solenoid
  */
  if (pooferStartTime == 0){
   pooferStartTime = millis;
  }
  else{
 Mills = millis;
  myMillis = Mills - pooferStartTime;
  //DEBUG("Called Poofer");
  //DEBUG(millis);
          if (myMillis > FINAL_POOF_OFF_MILLIS){
        digitalWrite (POOFER_PIN, LOW);
        poofComplete = true;
          }
     else if (myMillis > FINAL_POOF_ON_MILLIS){
digitalWrite (POOFER_PIN, HIGH);
intensity = 255;
SMOKE_SERIAL.print(intensity);
COMM_PRINT("printed Poof: ");
COMM_PRINTLN(intensity);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutRed);
COMM_PRINT("printed Red: ");
COMM_PRINTLN(spoutRed);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutBlue);
COMM_PRINT("printed Blue: ");
COMM_PRINTLN(spoutBlue);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.println(spoutGreen);
COMM_PRINT("printed Green: ");
COMM_PRINTLN(spoutGreen);
      }
       else if (myMillis > LONG_POOF_OFF_MILLIS){
digitalWrite (POOFER_PIN, LOW);
SMOKE_SERIAL.print(0);
COMM_PRINT("printed Poof: ");
COMM_PRINTLN(0);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutRed);
COMM_PRINT("printed Red: ");
COMM_PRINTLN(spoutRed);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutBlue);
COMM_PRINT("printed Blue: ");
COMM_PRINTLN(spoutBlue);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.println(spoutGreen);
COMM_PRINT("printed Green: ");
COMM_PRINTLN(spoutGreen);
       }
       else if (myMillis > LONG_POOF_ON_MILLIS){
digitalWrite (POOFER_PIN, HIGH);
intensity = 200;
SMOKE_SERIAL.print(intensity);
COMM_PRINT("printed Poof: ");
Serial.println(intensity);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutRed);
COMM_PRINT("printed Red: ");
COMM_PRINTLN(spoutRed);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutBlue);
COMM_PRINT("printed Blue: ");
COMM_PRINTLN(spoutBlue);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.println(spoutGreen);
COMM_PRINT("printed Green: ");
COMM_PRINTLN(spoutGreen);
       }
       else if (myMillis > MED_POOF_OFF_MILLIS){
digitalWrite (POOFER_PIN, LOW);
SMOKE_SERIAL.print(0);
COMM_PRINT("printed Poof: ");
COMM_PRINTLN(0);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutRed);
COMM_PRINT("printed Red: ");
COMM_PRINTLN(spoutRed);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutBlue);
COMM_PRINT("printed Blue: ");
COMM_PRINTLN(spoutBlue);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.println(spoutGreen);
COMM_PRINT("printed Green: ");
COMM_PRINTLN(spoutGreen);
       }
        else if (myMillis > MED_POOF_ON_MILLIS){
digitalWrite (POOFER_PIN, HIGH);
intensity = 120;
SMOKE_SERIAL.print(intensity);
COMM_PRINT("printed Poof: ");
COMM_PRINTLN(intensity);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutRed);
COMM_PRINT("printed Red: ");
COMM_PRINTLN(spoutRed);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutBlue);
COMM_PRINT("printed Blue: ");
COMM_PRINTLN(spoutBlue);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.println(spoutGreen);
COMM_PRINT("printed Green: ");
COMM_PRINTLN(spoutGreen);
       }
       else if (myMillis > SHORT_POOF_OFF_MILLIS){
digitalWrite (POOFER_PIN, LOW);
SMOKE_SERIAL.print(0);
COMM_PRINT("printed Poof: ");
COMM_PRINTLN(0);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutRed);
COMM_PRINT("printed Red: ");
COMM_PRINTLN(spoutRed);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutBlue);
COMM_PRINT("printed Blue: ");
COMM_PRINTLN(spoutBlue);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.println(spoutGreen);
COMM_PRINT("printed Green: ");
COMM_PRINTLN(spoutGreen);
       }
      else if (myMillis > SHORT_POOF_ON_MILLIS){
        digitalWrite (PILOT_PIN, HIGH);
        intensity = 122;
SMOKE_SERIAL.print(intensity);
COMM_PRINT("printed Poof: ");
COMM_PRINTLN(intensity);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutRed);
COMM_PRINT("printed Red: ");
COMM_PRINTLN(spoutRed);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutBlue);
COMM_PRINT("printed Blue: ");
COMM_PRINTLN(spoutBlue);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.println(spoutGreen);
COMM_PRINT("printed Green: ");
COMM_PRINTLN(spoutGreen);
      }
        else if(myMillis > SPARK_START_MILLIS){
        if (sparkerEvent == 0){
        SparkMaster();
        DEBUG("Sparker Start", millis);
        }
        }
        else if (myMillis > PILOT_START_MILLIS){
    digitalWrite (PILOT_PIN, HIGH);
      }
        else{
          DEBUG("got to poofer else");
        }
    }
  }
/** FINISH Poofer Modules Definitions ***/

/** BEGIN Smoke Modules Definitions ***/
void Smoke::setup() {
    SMOKE_SERIAL.begin(9600);
}
void Smoke::initialize() {
0;
     // XXX TODO XXX
}
void Smoke::Update(){
 // XXX TODO XXX
}
void Smoke::off(long touchEnded) {
//SPARK_END_MILLIS 2000 //stop spark ignitor
//PILOT_END_MILLIS 3000//stop pilot solenoid
SMOKE_SERIAL.print(0);
COMM_PRINT("printed Poof: ");
COMM_PRINTLN(0);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutRed);
COMM_PRINT("printed Red: ");
COMM_PRINTLN(spoutRed);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutBlue);
COMM_PRINT("printed Blue: ");
COMM_PRINTLN(spoutBlue);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.println(spoutGreen);
COMM_PRINT("printed Green: ");
COMM_PRINTLN(spoutGreen);
}
void Smoke::display(long millis) {
intensity = 255;
SMOKE_SERIAL.print(intensity);
COMM_PRINT("printed Poof: ");
COMM_PRINTLN(intensity);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutRed);
COMM_PRINT("printed Red: ");
COMM_PRINTLN(spoutRed);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.print(spoutBlue);
COMM_PRINT("printed Blue: ");
COMM_PRINTLN(spoutBlue);
SMOKE_SERIAL.print(",");
SMOKE_SERIAL.println(spoutGreen);
COMM_PRINT("printed Green: ");
COMM_PRINTLN(spoutGreen);
  }
/** FINISH Smoke Modules Definitions ***/
/** BEGIN BodyLEDs Modules Definitions ***/
void BodyLEDs::setup() {
   // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,LED_PIN_LEFT,COLOR_ORDER>(left, LEFT_NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,LED_PIN_RIGHT,COLOR_ORDER>(right, RIGHT_NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}
void BodyLEDs::initialize() {
  //FastLED.clear();
}
void BodyLEDs::off(long timeOff) {
  //FastLED.clear();
  if (timeCalledEnd == 0) {
    timeCalledEnd = millis();
    long timeOffEnd = LED_OFF_MILLIS + timeCalledEnd;
  }

  else if (millis() > timeOffEnd){
    //DEBUG ("got to BODYLEDS.off Elif");
    FastLED.clear();
    t.stop(FastUpdateTask);
  }
  else{
   long CurTime = millis() - timeOff;
  fade = constrain(map(CurTime, timeCalledEnd, timeOffEnd, brightCal, 0), 0, 255);
   //DEBUG (fade);
  FastLED.setBrightness(fade);
  // turn leds off     // XXX TODO XXX
  }
}
void BodyLEDs::display(long millis) {
  if (millis > LED_START_MILLIS){
  if (FastUpdateTask == 0){
    FastUpdateSetup();
  }
  fade = 0;
  timeEnd = timeCalled + SHORT_POOF_ON_MILLIS;
  brightCal= constrain(map(millis, timeCalled, timeEnd, 0, 255), 0, 255);
  spoutRed = brightCal;
  for(int i = 0; i < LEFT_NUM_LEDS; i++ )
   {
   left[i].setRGB(0,255,0);  // setRGB functions works by setting
    right[i].setRGB(0,255,0);
    //DEBUG ("MAXBRIGHT");
  }
    FastLED.setBrightness(brightCal);
    //DEBUG(FastLED.show);
    if (brightCal == 255) {
      return;
    }
  }
  else{
  }
}
/** FINISH BodyLEDs Modules Definitions ***/
/** FINISH Touch Modules Definitions ***/

/** FINISH Modules Definitions ***/
/*** FINISH Modules Interfaces***/




/*** BEGIN Control Initialization***/
Touch* touch;
Poofer* poofer;
Smoke* smoke;
BodyLEDs* bodyLEDs;
/*** FINISH Control Initialization***/




/*** BEGIN Setup Routine ***/
void setup() {
  touch = new Touch();
  poofer = new Poofer();
  smoke = new Smoke();
  bodyLEDs = new BodyLEDs();

  touch->setup();
  poofer->setup();
  smoke->setup();
  bodyLEDs->setup();
}
/*** FINISH Setup Routine ***/



/*** BEGIN Main operation entry point ***/
void loop() {
//DEBUG("beginning");
  // Initialize
  touch->initialize();
  poofer->initialize();
  //smoke->initialize();
  bodyLEDs->initialize();
//DEBUG(" after initialize");
//DEBUG(" after initialize");
  // Main operational loop
  long touched_millis;
  while (true) {
    t.update();
    touched_millis = touch->touched_time();
    if (!touched_millis) {
      //DEBUG("got to main if (nottouched)");
      poofer->off(touchEnded);
      //smoke->off(touchEnded);
      bodyLEDs->off(touchEnded);
      //DEBUG("shut off everything");
    }
    else  if (poofComplete == true){
 touch->initialize();
  poofer->initialize();
  //smoke->initialize();
  bodyLEDs->initialize();
  //DEBUG("got to main re-initialize");
 /*       touched_millis = 0;
    minRead = 0;
    memset(firstRead, 0, sizeof(firstRead));
    //confidence = 0;
    FastLED.clear();
    FastLED.show();
    t.stop(FastUpdateTask);
    poofComplete = false;
    DEBUG("POOFER THING", poofComplete, touched_millis);
    */
    }
    else {
      //DEBUG("got to main else(touched)");
      // XXX TODO XXX See if we should progress to the next output stage
      //DEBUG(" being touched ");
      poofer->display(touched_millis);
      bodyLEDs->display(touched_millis);
      //smoke->display(touched_millis);
      }
      //DEBUG(" after initialize");
    }

}
/*** FINISH Main operation entry point ***/

