/*** control.ino
 *      Master control for the genie lamp project
 ***/


/*** BEGIN Constants ***/
// XXX TODO XXX Add constants not folded into subsystem classes
/*** FINISH Constants ***/


/*** BEGIN Config ***/
// XXX TODO XXX Add steps for what happens to each output when the lamp has been rubbed for a given amount of time
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

class InputBase : public ModuleBase
{
    public:
        virtual int touched_time() = 0;
};

class Touch : public InputBase
{
    public:
        virtual void setup();
        virtual void initialize();
        virtual int touched_time();
    private:
        int _touched_millis = 0;
};

class OutputBase : public ModuleBase
{
    public:
        virtual void off() = 0;
        virtual void display(int millis) = 0;
};

class Poofer : public OutputBase
{
    public:
        virtual void setup();
        virtual void initialize();
        virtual void off();
        virtual void display(int millis);
};

class BodyLEDs : public OutputBase
{
    public:
        virtual void setup();
        virtual void initialize();
        virtual void off();
        virtual void display(int millis);
};
  /** FINISH Modules Declarations ***/
    
  /** BEGIN Modules Definitions ***/
    /** BEGIN Touch Modules Definitions ***/
    void Touch::setup() {
        0;      // XXX TODO XXX
    }
    void Touch::initialize() {
        0;      // XXX TODO XXX
    }
    int Touch::touched_time() {
        return 0;      // XXX TODO XXX
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
    void Poofer::display(int millis) {
        0;      // XXX TODO XXX
    }
    /** FINISH Poofer Modules Definitions ***/

    /** BEGIN BodyLEDs Modules Definitions ***/
    void BodyLEDs::setup() {
        0;      // XXX TODO XXX
    }
    void BodyLEDs::initialize() {
        0;      // XXX TODO XXX
    }
    void BodyLEDs::off() {
        0;      // XXX TODO XXX
    }
    void BodyLEDs::display(int millis) {
        0;      // XXX TODO XXX
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
    int touched_millis;
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

