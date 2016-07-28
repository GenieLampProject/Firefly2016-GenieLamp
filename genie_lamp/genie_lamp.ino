// uncomment the following to disable serial debug statements
//#define SERIAL_DEBUG false
/**Uncomment below line to turn off Communication messages. **/
//#define COMM false
/*
* Utility functions to help debugging running code.
*/



#include <SerialDebug.h>
#include "Timer.h"
#include "FastLED.h"



/*** BEGIN Macros ***/
#ifdef COMM
#define COMM_PRINT(x)  Serial.print (x)
#define COMM_PRINTLN(x)  Serial.println (x)
#else
#define COMM_PRINT(x)
#define COMM_PRINTLN(x)
#endif

// XXX QUESTION XXX Make runtime changeable?
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
/*** FINISH Macros ***/


/*** BEGIN Constants ***/
#define LED_TYPE        WS2812
#define COLOR_ORDER     RGB

#define SMOKE_SERIAL Serial2

#define FRAMES_PER_SECOND  120
/*** FINISH Constants ***/


/*** BEGIN Globals ***/
// Master timer/interrupt/multi-thread control
Timer timer;
int8_t FastUpdateTask = 0;   // int8_t to match return Type of Timer.every()
//// Runtime touch data members
long touchEnded = 0;      // XXX TODO XXX Modularize
//// Runtime smoke data members
int spoutRed;             // XXX TODO XXX Modularize

long timeCalled = 0;      // XXX TODO XXX Move to appropriate scope
long timeCalledEnd = 0;   // XXX TODO XXX Move to appropriate scope
int8_t singleSparkEvent = 0;  // int8_t to match return Type of Timer.pulse()   // XXX TODO XXX Move to appropriate scope
/*** FINISH Globals ***/


/*** BEGIN Config ***/
/* when lamp is rubbed for (in millis):*/
#define LED_START_MILLIS 2000 //Begin LEDS
#define PILOT_START_MILLIS 8000 //open pilot solenoid
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
        //// Touch sensor constants
        // Sensitivity knobs
        static const int sensitivity = 400;
        // Touch sensor pin constants
        static const byte pins = 6;
        // Left side
        static const byte TOUCH_LEFT_FRONT_PIN = 16;
        static const byte TOUCH_LEFT_MIDDLE_PIN = 17;
        static const byte TOUCH_LEFT_BACK_PIN = 18;
        // Right side
        static const byte TOUCH_RIGHT_FRONT_PIN = 15;
        static const byte TOUCH_RIGHT_MIDDLE_PIN = 1;
        static const byte TOUCH_RIGHT_BACK_PIN = 0; // PLEASE NOTE
        const byte touchPin[Touch::pins] = {
                this->TOUCH_LEFT_FRONT_PIN,
                this->TOUCH_LEFT_MIDDLE_PIN,
                this->TOUCH_LEFT_BACK_PIN,
                this->TOUCH_RIGHT_FRONT_PIN,
                this->TOUCH_RIGHT_MIDDLE_PIN,
                this->TOUCH_RIGHT_BACK_PIN };

        //// Runtime touch sensor data members
        int attempts = 0;
        int tolerance = 300;
        int confidence = 0;
        int read1[Touch::pins];
        long firstRead[Touch::pins] = { 0 };
        long minRead = 0;
        int base[Touch::pins];
        long lastBase[Touch::pins];     // NB: Unused
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
        virtual void update();                  // NB: Unused
        virtual void spark();
        virtual void sparkMaster();
        virtual void poof(long duration);       // NB: Unused
        virtual bool get_poof_complete();
        //// Poofer constants
        // Poofer pin constants
        static const byte SPARKER_PIN = 5;      // Same as Smoke.BLUE_PIN
    private:
        static const byte PILOT_PIN = 3;        // Same as Smoke.GREEN_PIN
        static const byte POOFER_PIN = 4;       // Same as Smoke.RED_PIN

        //// Runtime poofer data members
        long pooferStartTime = 0;
        bool sparkerEvent = false;
        int8_t pooferEvent = 0;       // int8_t to match return Type of Timer.pulse()
        int8_t allsparkEvent = 0;     // int8_t to match return Type of Timer.every()
        bool resting = false;         // NB: Unused
        bool poofComplete = false;
        long myMillis = 0;
        long totalMills = 0;          // NB: Unused
        long OffDiff = 0;

        //// Runtime smoke data members
//        int spoutRed;
        int spoutBlue;     // NB: Never assigned
        int spoutGreen;    // NB: Never assigned
        int intensity;
};

class BodyLEDs : public OutputBase
{
    public:
        virtual void setup();
        virtual void initialize();
        virtual void off(long timeOff);
        virtual void display(long millis);
        virtual void fastUpdate();
        virtual void fastUpdateSetup();
    private:
        //// Body LED constants
        static const byte BRIGHTNESS = 50;
        // Body LED pin constants
        static const byte LED_LEFT_PIN = 22;
        static const byte LED_RIGHT_PIN = 21;
        static const byte LEFT_NUM_LEDS = 106;
        static const byte RIGHT_NUM_LEDS = 106;

        //// Runtime body LED data members
        CRGB left_leds[BodyLEDs::LEFT_NUM_LEDS];
        CRGB right_leds[BodyLEDs::RIGHT_NUM_LEDS];
        uint8_t gHue = 0; // rotating "base color" used by many of the patterns     // NB: Unused
        int fade = 0;
        int timeTarget = 10000;     // NB: Unused
        int brightCal = 0;
        long timeCalledStart = 0;     // NB: Unused
        long timeOffEnd = 0;
        long timeEnd = 0;
};

class Smoke : public OutputBase
{
    public:
        virtual void setup();
        virtual void initialize();
        virtual void off(long timeOff);
        virtual void display(long millis);
        virtual void update();                   // NB: Unused
    private:
        //// Smoke constants
        // Smoke pin constants
        static const byte GREEN_PIN = 3;      // Same as Poofer.PILOT_PIN
        static const byte RED_PIN = 4;        // Same as Poofer.POOFER_PIN
        static const byte BLUE_PIN = 5;       // Same as Poofer.SPARKER_PIN

        //// Runtime smoke data members
//        int spoutRed;
        int spoutBlue;     // NB: Never assigned
        int spoutGreen;    // NB: Never assigned
        int intensity;
};

/** BEGIN Output Modules Declarations ***/
/** FINISH Modules Declarations ***/

/** BEGIN Modules Definitions ***/
/** BEGIN Touch Modules Definitions ***/
void Touch::setup() {
    SERIAL_DEBUG_SETUP(9600);
        //"touch setup Called");
    //takes a baseline reading for the sensor.
    byte bpin;
    for (bpin = 0; bpin < this->pins; bpin++) {
        this->base[bpin] = touchRead(this->touchPin[bpin]); // Baseline calibration
        this->lastBase[bpin] = millis();     // NB: Unused
        //DEBUG(this->base[bpin]);
    }
}

void Touch::initialize() { }

long Touch::touched_time() {
    //DEBUG("touched_time Called");
    //returns a long from the sensors that tells the duration of the touch
    timeCalled = millis();
    bool touched = false;
    this->attempts = constrain(this->attempts, 0, 200);
    this->confidence = max (this->confidence, 0);
    byte readPin;
    long result;
    long totalTime;
    long minReadResult = 0;
    result = 0;
    long readTime[this->pins] = { 0 };
    long timeDiff[this->pins] = { 0 };
    for (readPin = 0; readPin < this->pins; readPin++) {
        String sensorNum = readPin;
        int Difference = this->read1[readPin] - this->base[readPin];
        // DEBUG(Difference);
        readTime[readPin] = millis();
        //get the time that the touch was read.
        //calculated the time difference for each read pin.
        this->read1[readPin] = touchRead(this->touchPin[readPin]);
        //sample the read pin to detect touch.
        if (Difference > this->sensitivity) {
            touched = true;
            this->confidence++;
            this->attempts = this->tolerance + this->confidence;
            if (minReadResult == 0) {
                minReadResult = readTime[readPin];
            }
            if (this->firstRead[readPin] == 0) {
              this->firstRead[readPin] = readTime[readPin];
            } else if (minReadResult > this->firstRead[readPin]) {
               minReadResult = this->firstRead[readPin];
            }
            if (timeDiff[readPin] > result) {
                result = timeDiff[readPin];
            }
        } else {
            // DEBUG(" PIN EMPTY ");
        }
    }
    // DEBUG(minReadResult);
    //DEBUG(this->minRead);
    if (touched == true) {
        touchEnded = 0;
        timeCalledEnd = 0;
        if (minReadResult <= this->minRead) {
            this->minRead = minReadResult;
        }
        totalTime = millis() - this->minRead;
        // DEBUG(" total time: ", totalTime);
        // DEBUG(" total time: ", this->minRead);
        return totalTime;
    } else if (touched == false) {
        if (this->attempts != 0) {

            this->attempts--;
            this->confidence = 0;
            // DEBUG("using attempt", this->attempts, this->confidence);
            return totalTime;
        } else if (this->attempts == 0) {
            result = 0;
            minReadResult = 0;
            memset(this->firstRead, 0, sizeof(this->firstRead));
            this->confidence = 0;
            this->minRead = millis();
            // DEBUG("No Touch");
            if (touchEnded == 0) {
                touchEnded = millis();
            }
            return false;
        }
    }
}
/** FINISH Touch Modules Definitions ***/

/** BEGIN Poofer Modules Definitions ***/
void Poofer::setup() {
    pinMode (this->SPARKER_PIN, OUTPUT);
    digitalWrite (this->SPARKER_PIN, LOW);
    pinMode (this->POOFER_PIN, OUTPUT);
    digitalWrite (this->POOFER_PIN, LOW);
    pinMode (this->PILOT_PIN, OUTPUT);
    digitalWrite (this->PILOT_PIN, LOW);
}

void Poofer::initialize() {
    this->pooferStartTime = 0;
    timer.stop(this->allsparkEvent);
    timer.stop(singleSparkEvent);
    digitalWrite (this->SPARKER_PIN, LOW);
    digitalWrite (this->POOFER_PIN, LOW);
    digitalWrite (this->PILOT_PIN, LOW);
    this->sparkerEvent = false;
    this->allsparkEvent = 0;
    singleSparkEvent = 0;

        // XXX TODO XXX
}

void Poofer::update() {                   // NB: Unused
    // perform a poof of specified duration
    timer.update();
}
bool Poofer::get_poof_complete() {
    return this->poofComplete;
}

void Poofer::off(long touchEnded) {
    this->poofComplete = false;
    //SPARK_END_MILLIS 2000 //stop spark ignitor
    //PILOT_END_MILLIS 3000//stop pilot solenoid
    digitalWrite(this->POOFER_PIN, LOW);
    SMOKE_SERIAL.print(0);
    COMM_PRINT("printed poof: ");
    COMM_PRINTLN(0);
    SMOKE_SERIAL.print(",");
    SMOKE_SERIAL.print(spoutRed);
    COMM_PRINT("printed Red: ");
    COMM_PRINTLN(spoutRed);
    SMOKE_SERIAL.print(",");
    SMOKE_SERIAL.print(this->spoutBlue);
    COMM_PRINT("printed Blue: ");
    COMM_PRINTLN(this->spoutBlue);
    SMOKE_SERIAL.print(",");
    SMOKE_SERIAL.println(this->spoutGreen);
    COMM_PRINT("printed Green: ");
    COMM_PRINTLN(this->spoutGreen);
    timer.stop(this->pooferEvent);
    this->pooferEvent = 0;
    this->OffDiff = millis() - touchEnded;
    DEBUG("offDiff", this->OffDiff, SPARK_END_MILLIS, this->allsparkEvent, touchEnded);
    if (this->OffDiff >= SPARK_END_MILLIS && this->allsparkEvent != 0) {
        DEBUG("terminate sparker");
        this->allsparkEvent = 0;
        timer.stop(this->allsparkEvent);
        timer.stop(singleSparkEvent);
        digitalWrite (this->SPARKER_PIN, LOW);
        this->sparkerEvent = false;
        if (this->OffDiff > PILOT_END_MILLIS) {
            digitalWrite (this->PILOT_PIN, LOW);
        }
    }
}

void Poofer::display(long millis) {
/*
 * when lamp is rubbed for (in millis):
#define LED_START_MILLIS 2000 //Begin LEDS
#define PILOT_START_MILLIS 8000 //open pilot solenoid
#define SPARK_START_MILLIS 8250 //start spark ignitor (.5 second duty)
#define SHORT_POOF_ON_MILLIS 10000//open poof solenoid
#define SHORT_POOF_OFF_MILLIS 10100//close poof solenoid
#define MED_POOF_ON_MILLIS 10500//open poof solenoid
#define MED_POOF_OFF_MILLIS 10700//close poof solenoid
#define LONG_POOF_ON_MILLIS 10900//open poof solenoid
#define LONG_POOF_OFF_MILLIS 11200//close poof solenoid
#define FINAL_POOF_ON_MILLIS 11400//open poof solenoid
#define FINAL_POOF_OFF_MILLIS 13000//close poof solenoid
 * when lamp NOT rubbed for:
#define LED_END_MILLIS 500 //Begin LED Fade
#define SPARK_END_MILLIS 2000 //stop spark ignitor
#define SPARK_END_MILLIS 3000//stop pilot solenoid
 */
    if (this->pooferStartTime == 0) {
        this->pooferStartTime = millis;
    } else {
        this->totalMills = millis;
        this->myMillis = millis - this->pooferStartTime;
        //DEBUG("Called Poofer");
        //DEBUG(millis);
        if (this->myMillis > FINAL_POOF_OFF_MILLIS) {
            digitalWrite (this->POOFER_PIN, LOW);
            this->poofComplete = true;
        } else if (this->myMillis > FINAL_POOF_ON_MILLIS) {
            digitalWrite (this->POOFER_PIN, HIGH);
            this->intensity = 255;
            SMOKE_SERIAL.print(this->intensity);
            COMM_PRINT("printed Poof: ");
            COMM_PRINTLN(this->intensity);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.print(spoutRed);
            COMM_PRINT("printed Red: ");
            COMM_PRINTLN(spoutRed);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.print(this->spoutBlue);
            COMM_PRINT("printed Blue: ");
            COMM_PRINTLN(this->spoutBlue);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.println(this->spoutGreen);
            COMM_PRINT("printed Green: ");
            COMM_PRINTLN(this->spoutGreen);
        } else if (this->myMillis > LONG_POOF_OFF_MILLIS) {
            digitalWrite (this->POOFER_PIN, LOW);
            SMOKE_SERIAL.print(0);
            COMM_PRINT("printed Poof: ");
            COMM_PRINTLN(0);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.print(spoutRed);
            COMM_PRINT("printed Red: ");
            COMM_PRINTLN(spoutRed);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.print(this->spoutBlue);
            COMM_PRINT("printed Blue: ");
            COMM_PRINTLN(this->spoutBlue);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.println(this->spoutGreen);
            COMM_PRINT("printed Green: ");
            COMM_PRINTLN(this->spoutGreen);
        } else if (this->myMillis > LONG_POOF_ON_MILLIS) {
            digitalWrite (this->POOFER_PIN, HIGH);
            this->intensity = 200;
            SMOKE_SERIAL.print(this->intensity);
            COMM_PRINT("printed Poof: ");
            Serial.println(this->intensity);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.print(spoutRed);
            COMM_PRINT("printed Red: ");
            COMM_PRINTLN(spoutRed);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.print(this->spoutBlue);
            COMM_PRINT("printed Blue: ");
            COMM_PRINTLN(this->spoutBlue);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.println(this->spoutGreen);
            COMM_PRINT("printed Green: ");
            COMM_PRINTLN(this->spoutGreen);
        } else if (this->myMillis > MED_POOF_OFF_MILLIS) {
            digitalWrite (this->POOFER_PIN, LOW);
            SMOKE_SERIAL.print(0);
            COMM_PRINT("printed Poof: ");
            COMM_PRINTLN(0);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.print(spoutRed);
            COMM_PRINT("printed Red: ");
            COMM_PRINTLN(spoutRed);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.print(this->spoutBlue);
            COMM_PRINT("printed Blue: ");
            COMM_PRINTLN(this->spoutBlue);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.println(this->spoutGreen);
            COMM_PRINT("printed Green: ");
            COMM_PRINTLN(this->spoutGreen);
        } else if (this->myMillis > MED_POOF_ON_MILLIS) {
            digitalWrite (this->POOFER_PIN, HIGH);
            this->intensity = 120;
            SMOKE_SERIAL.print(this->intensity);
            COMM_PRINT("printed Poof: ");
            COMM_PRINTLN(this->intensity);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.print(spoutRed);
            COMM_PRINT("printed Red: ");
            COMM_PRINTLN(spoutRed);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.print(this->spoutBlue);
            COMM_PRINT("printed Blue: ");
            COMM_PRINTLN(this->spoutBlue);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.println(this->spoutGreen);
            COMM_PRINT("printed Green: ");
            COMM_PRINTLN(this->spoutGreen);
        } else if (this->myMillis > SHORT_POOF_OFF_MILLIS) {
            digitalWrite (this->POOFER_PIN, LOW);
            SMOKE_SERIAL.print(0);
            COMM_PRINT("printed Poof: ");
            COMM_PRINTLN(0);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.print(spoutRed);
            COMM_PRINT("printed Red: ");
            COMM_PRINTLN(spoutRed);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.print(this->spoutBlue);
            COMM_PRINT("printed Blue: ");
            COMM_PRINTLN(this->spoutBlue);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.println(this->spoutGreen);
            COMM_PRINT("printed Green: ");
            COMM_PRINTLN(this->spoutGreen);
        } else if (this->myMillis > SHORT_POOF_ON_MILLIS) {
            digitalWrite (this->PILOT_PIN, HIGH);
            this->intensity = 122;
            SMOKE_SERIAL.print(this->intensity);
            COMM_PRINT("printed Poof: ");
            COMM_PRINTLN(this->intensity);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.print(spoutRed);
            COMM_PRINT("printed Red: ");
            COMM_PRINTLN(spoutRed);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.print(this->spoutBlue);
            COMM_PRINT("printed Blue: ");
            COMM_PRINTLN(this->spoutBlue);
            SMOKE_SERIAL.print(",");
            SMOKE_SERIAL.println(this->spoutGreen);
            COMM_PRINT("printed Green: ");
            COMM_PRINTLN(this->spoutGreen);
        } else if (this->myMillis > SPARK_START_MILLIS) {
            if (!this->sparkerEvent) {
                this->sparkMaster();
                DEBUG("Sparker Start", millis);
            }
        } else if (this->myMillis > PILOT_START_MILLIS) {
            digitalWrite (this->PILOT_PIN, HIGH);
        } else {
              DEBUG("got to poofer else");
        }
    }
}


// Helper callback to match the Timer.h 
void spark_callback() {
    DEBUG("Called spark");
    singleSparkEvent = timer.pulse(Poofer::SPARKER_PIN, SPARKER_ON_TIME, HIGH);
}

void Poofer::spark() {       // XXX JGF XXX TODO XXX Get Timer.h library playing nice with classes and replace spark_callback() XXX TODO XXX JGF XXX
    DEBUG("Called spark");
//    this->singleSparkEvent = timer.pulse(this->SPARKER_PIN, SPARKER_ON_TIME, HIGH);
}

void Poofer::sparkMaster() {
    DEBUG("Called sparkMaster");
    if (!this->sparkerEvent) {
        this->allsparkEvent = timer.every(SPARKER_OFF_TIME, spark_callback);
//        this->allsparkEvent = timer.every(SPARKER_OFF_TIME, this->spark);       // XXX JGF XXX TODO XXX Get Timer.h library playing nice with classes and replace spark_callback() XXX TODO XXX JGF XXX
    }
    this->sparkerEvent = true;
}

void Poofer::poof(long duration) {          // NB: Unused
    // perform a poof of specified duration
    this->pooferEvent = timer.pulse(this->POOFER_PIN, duration, HIGH);
}


/** FINISH Poofer Modules Definitions ***/

/** BEGIN Smoke Modules Definitions ***/
void Smoke::setup() {
        SMOKE_SERIAL.begin(38400);
}

void Smoke::initialize() {
0;
// XXX TODO XXX
}

void Smoke::update() {                   // NB: Unused
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
    SMOKE_SERIAL.print(this->spoutBlue);
    COMM_PRINT("printed Blue: ");
    COMM_PRINTLN(this->spoutBlue);
    SMOKE_SERIAL.print(",");
    SMOKE_SERIAL.println(this->spoutGreen);
    COMM_PRINT("printed Green: ");
    COMM_PRINTLN(this->spoutGreen);
}

void Smoke::display(long millis) {
    this->intensity = 255;
    SMOKE_SERIAL.print(this->intensity);
    COMM_PRINT("printed Poof: ");
    COMM_PRINTLN(this->intensity);
    SMOKE_SERIAL.print(",");
    SMOKE_SERIAL.print(spoutRed);
    COMM_PRINT("printed Red: ");
    COMM_PRINTLN(spoutRed);
    SMOKE_SERIAL.print(",");
    SMOKE_SERIAL.print(this->spoutBlue);
    COMM_PRINT("printed Blue: ");
    COMM_PRINTLN(this->spoutBlue);
    SMOKE_SERIAL.print(",");
    SMOKE_SERIAL.println(this->spoutGreen);
    COMM_PRINT("printed Green: ");
    COMM_PRINTLN(this->spoutGreen);
}

/** FINISH Smoke Modules Definitions ***/
/** BEGIN BodyLEDs Modules Definitions ***/
void BodyLEDs::setup() {
    // tell FastLED about the LED strip configuration
    FastLED.addLeds<LED_TYPE, this->LED_LEFT_PIN, COLOR_ORDER>(this->left_leds, this->LEFT_NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, this->LED_RIGHT_PIN, COLOR_ORDER>(this->right_leds, this->RIGHT_NUM_LEDS).setCorrection(TypicalLEDStrip);
    //FastLED.addLeds<LED_TYPE, DATA_PIN, CLK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    // set master brightness control
    FastLED.setBrightness(this->BRIGHTNESS);
}

void BodyLEDs::initialize() {
    //FastLED.clear();
}

void BodyLEDs::off(long timeOff) {
    //FastLED.clear();
    if (timeCalledEnd == 0) {
        timeCalledEnd = millis();
        this->timeOffEnd = LED_OFF_MILLIS + timeCalledEnd;
    } else if (millis() > this->timeOffEnd) {
        //DEBUG ("got to BODYLEDS.off Elif");
        FastLED.clear();
        timer.stop(FastUpdateTask);
    } else {
        long CurTime = millis() - timeOff;
        this->fade = constrain(map(CurTime, timeCalledEnd, this->timeOffEnd, this->brightCal, 0), 0, 255);
        //DEBUG (this->fade);
        FastLED.setBrightness(this->fade);
        // turn leds off     // XXX TODO XXX
    }
}

void BodyLEDs::display(long millis) {
    if (millis > LED_START_MILLIS) {
        if (FastUpdateTask == 0) {
            this->fastUpdateSetup();
        }
        this->fade = 0;
        this->timeEnd = timeCalled + SHORT_POOF_ON_MILLIS;
        this->brightCal= constrain(map(millis, timeCalled, this->timeEnd, 0, 255), 0, 255);
        spoutRed = this->brightCal;
        for(int i = 0; i < this->LEFT_NUM_LEDS; i++ )
        {
            this->left_leds[i].setRGB(0, 255, 0);  // setRGB functions works by setting
            this->right_leds[i].setRGB(0, 255, 0);
            //DEBUG ("MAXBRIGHT");
        }
        FastLED.setBrightness(this->brightCal);
        //DEBUG(FastLED.show);
        if (this->brightCal == 255) {
            return;
        }
    } else { }
}


void fastUpdate_callback() {
    FastLED.show();
    //DEBUG(" showing FASTLED");
}

void BodyLEDs::fastUpdate() {       // XXX JGF XXX TODO XXX Get Timer.h library playing nice with classes and replace spark_callback() XXX TODO XXX JGF XXX
    FastLED.show();
    //DEBUG(" showing FASTLED");
}

void BodyLEDs::fastUpdateSetup() {
    FastUpdateTask = timer.every(1000/FRAMES_PER_SECOND, fastUpdate_callback);
//    FastUpdateTask = timer.every(1000/FRAMES_PER_SECOND, this->fastUpdate);       // XXX JGF XXX TODO XXX Get Timer.h library playing nice with classes and replace spark_callback() XXX TODO XXX JGF XXX
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
        timer.update();

        touched_millis = touch->touched_time();
        if (!touched_millis) {
            //DEBUG("got to main if (nottouched)");
            poofer->off(touchEnded);
            //smoke->off(touchEnded);
            bodyLEDs->off(touchEnded);
            //DEBUG("shut off everything");
        } else if (poofer->get_poof_complete()) {
            touch->initialize();
            poofer->initialize();
            //smoke->initialize();
            bodyLEDs->initialize();
            //DEBUG("got to main re-initialize");
/*            touched_millis = 0;
            touch->minRead = 0;
            memset(touch->firstRead, 0, sizeof(touch->firstRead));
            //touch->confidence = 0;
            FastLED.clear();
            FastLED.show();
            timer.stop(FastUpdateTask);
            poofer->poofComplete = false;
            DEBUG("POOFER THING", poofer->get_poof_complete(), touched_millis);
 */
        } else {
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

