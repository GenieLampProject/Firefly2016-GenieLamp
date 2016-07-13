/*** control.ino
 *      Master control for the genie lamp project
 ***/


#include <FastLED.h>


/*** BEGIN Macros ***/
// XXX QUESTION XXX Make runtime changeable?
#ifdef DEBUG
  #define DEBUG_SERIAL(x)  Serial.begin(x)
  #define DEBUG_PRINT(x)  Serial.print (x)
  #define DEBUG_PRINTLN(x)  Serial.println (x)
#else
  #define DEBUG_SERIAL(x)
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif
/*** FINISH Macros ***/


/*** BEGIN Constants ***/
#define LED_TYPE     WS2812
#define COLOR_ORDER RGB
/*** FINISH Constants ***/


// XXX TODO XXX Add to the appropriate specific .h/.cpp files
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
        static const int pins = 6;

        long _touched_millis = 0;
        int touchPin[pins] = {0, 1, 15, 16, 17, 18};
        /*Needed for readings*/
        int sensitivity = 400;
        int read1[pins];
        long firstRead[pins];
        /*needed for baseline*/
        int base[pins];
        long lastBase[pins];
};
    /** FINISH Input Modules Declarations ***/

    /** BEGIN Output Modules Declarations ***/
class OutputBase : public ModuleBase
{
    public:
        virtual void off() = 0;
        virtual void display(long millis) = 0;
};

class Poofer : public OutputBase
{
    public:
        virtual void setup();
        virtual void initialize();
        virtual void off();
        virtual void display(long millis);
};

class BodyLEDs : public OutputBase
{
    public:
        virtual void setup();
        virtual void initialize();
        virtual void off();
        virtual void display(long millis);
    private:
        static const int BRIGHTNESS = 50;

        static const int LEFT_PIN = 22;
        static const int RIGHT_PIN = 21;
        static const int LEFT_NUM_LEDS = 200;
        static const int RIGHT_NUM_LEDS = 200;

        CRGB left[LEFT_NUM_LEDS];
        CRGB right[RIGHT_NUM_LEDS];
        long timeCalled = 0;
        long timeEnd = 0;
};
    /** BEGIN Output Modules Declarations ***/
  /** FINISH Modules Declarations ***/


  /** BEGIN Modules Definitions ***/
    /** BEGIN Touch Modules Definitions ***/
    void Touch::setup() {
        //takes a baseline reading for the sensor.
        DEBUG_SERIAL(38400);
        for (int bpin = 0; bpin < this->pins; bpin++) {
            this->base[bpin] = touchRead(this->touchPin[bpin]); // Baseline calibration
            this->lastBase[bpin] = millis();
            DEBUG_PRINTLN(this->base[bpin]);
        }
    }
    void Touch::initialize() {
        0;      // XXX TODO XXX
    }
    long Touch::touched_time() {
        //#define DEBUG
        //returns a long from the sensor that has been touched for the longest time. l with the duration of the touch  consecutively for the longest time.
        int readPin;
        long result;
        long minRead;
        long totalTime;
        result = 0;
        long readTime[this->pins] = {0};
        long timeDiff[this->pins] = {0};
        for (readPin = 0; readPin < this->pins; readPin++) {
            int Difference = this->read1[readPin] - this->base[readPin];
            readTime[readPin] = millis();
            timeDiff[readPin] = readTime[readPin] - this->firstRead[readPin];
            this->read1[readPin] = touchRead(this->touchPin[readPin]);
            if (Difference > this->sensitivity && this->firstRead[readPin] == 0) {
                this->firstRead[readPin] = readTime[readPin];
                if (this->firstRead[readPin] < minRead) {
                    minRead = this->firstRead[readPin];
                    DEBUG_PRINT(" on for: "); DEBUG_PRINT(timeDiff[readPin]); DEBUG_PRINT(" Baseval: "); DEBUG_PRINT(this->base[readPin]); DEBUG_PRINT("  Read: "); DEBUG_PRINT(this->read1[readPin]); DEBUG_PRINT(" BL diff: "); DEBUG_PRINT(Difference); DEBUG_PRINT(" Threshold: "); DEBUG_PRINT(this->sensitivity); DEBUG_PRINTLN();
                }
            } else if (Difference > this->sensitivity && timeDiff[readPin] <= 20000) {
                //If a single pin has been on for more than two seconds, set it to 0
                this->firstRead[readPin] = 0;
            } else if (Difference > this->sensitivity && timeDiff[readPin] >= 5) {
                //If the difference is more than sensativity and the millis of the current read is .005 seconds greater than the first touch read.
                if (timeDiff[readPin] > result) {
                    result = timeDiff[readPin];
                    DEBUG_PRINT(" on for: "); DEBUG_PRINT(timeDiff[readPin]); DEBUG_PRINT(" Baseval: "); DEBUG_PRINT(this->base[readPin]); DEBUG_PRINT("  Read: "); DEBUG_PRINT(this->read1[readPin]); DEBUG_PRINT(" BL diff: "); DEBUG_PRINT(Difference); DEBUG_PRINT(" Threshold: "); DEBUG_PRINT(this->sensitivity); DEBUG_PRINTLN();
                }
            }
            /**   begin sense any touch
            else if (Difference > this->sensitivity && this->firstRead[readPin] != 0) {
                if (timeDiff[readPin] > result) {
                   result = timeDiff[readPin];
                 };
            }
            end sense any touch **/
            else {
                this->firstRead[readPin] = 0;
            }
        }
        if (result != 0) {
            totalTime = totalTime - millis();
            return totalTime;
        } else {
            return false;
        }
    }
    /** FINISH Touch Modules Definitions ***/

    /** BEGIN Poofer Modules Definitions ***/
    void Poofer::setup() {
        0;      // XXX TODO XXX
    }
    void Poofer::initialize() {
        0;      // XXX TODO XXX
    }
    void Poofer::off() {
        0;      // XXX TODO XXX
    }
    void Poofer::display(long millis) {
        0;      // XXX TODO XXX
    }
    /** FINISH Poofer Modules Definitions ***/

    /** BEGIN BodyLEDs Modules Definitions ***/
    void BodyLEDs::setup() {
        // tell FastLED about the LED strip configuration
        FastLED.addLeds<LED_TYPE, LEFT_PIN, COLOR_ORDER>(this->left, this->LEFT_NUM_LEDS).setCorrection(TypicalLEDStrip);
        FastLED.addLeds<LED_TYPE, RIGHT_PIN, COLOR_ORDER>(this->right, this->RIGHT_NUM_LEDS).setCorrection(TypicalLEDStrip);
        //FastLED.addLeds<LED_TYPE, DATA_PIN, CLK_PIN, this->COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

        // set master brightness control
        FastLED.setBrightness(this->BRIGHTNESS);
    }
    void BodyLEDs::initialize() {
        this->timeCalled = millis();    // XXX QUESTION XXX What does "timeCalled" do? Shouldn't it always be 0?
    }
    void BodyLEDs::off() {
        FastLED.clear();
        // turn leds off
        this->timeCalled = 0;      // XXX TODO XXX    // XXX QUESTION XXX What does "timeCalled" do? Shouldn't it always be 0?
    }
    void BodyLEDs::display(long millis) {
        long brightCalc = constrain(map(millis, this->timeCalled, this->timeEnd, 0, 255), 0, 255);    // XXX QUESTION XXX What does "timeCalled"/"timeEnnd" do? Shouldn't they always be 0 with this code?
        for(int i = 0; i < this->LEFT_NUM_LEDS; i++)
        {
            this->left[i].setRGB(0, 255, 0);  // setRGB functions works by setting
            this->right[i].setRGB(0, 255, 0);
        }
        FastLED.setBrightness(brightCalc);
        if (brightCalc == 255) {
            return;
        }
    }
    /** FINISH BodyLEDs Modules Definitions ***/
    /** FINISH Touch Modules Definitions ***/

  /** FINISH Modules Definitions ***/
/*** FINISH Modules Interfaces***/




/*** BEGIN Control Initialization***/
Touch* touch;
Poofer* poofer;
BodyLEDs* bodyLEDs;
/*** FINISH Control Initialization***/




/*** BEGIN Setup Routine ***/
void setup() {
    touch = new Touch();
    poofer = new Poofer();
    bodyLEDs = new BodyLEDs();

    touch->setup();
    poofer->setup();
    bodyLEDs->setup();
}
/*** FINISH Setup Routine ***/




/*** BEGIN Main operation entry point ***/
void loop() {
    // Initialize
    touch->initialize();
    poofer->initialize();
    bodyLEDs->initialize();


    // Main operational loop
    long touched_millis;
    while (true) {
        touched_millis = touch->touched_time();
        if (!touched_millis) {
            poofer->off();
            bodyLEDs->off();
        } else {
            // XXX TODO XXX See if we should progress to the next output stage
            poofer->display(touched_millis);
            bodyLEDs->display(touched_millis);
        }
    }

}
/*** FINISH Main operation entry point ***/

