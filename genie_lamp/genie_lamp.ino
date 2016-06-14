/*** control.ino
 *      Master control for the genie lamp project
 ***/


/*** BEGIN Constants ***/
// XXX TODO XXX Add constants not folded into subsystem classes
/*** FINISH Constants ***/


/*** BEGIN Config ***/
// XXX TODO XXX Add steps for what happens to each output when the lamp has been rubbed for a given amount of time
/*** FINISH Config ***/


/*** BEGIN Modules Interfaces***/
class ModuleBase
{
    public:
        void setup();
        void initialize();
};

class InputBase : public ModuleBase
{
    public:
        int touched_time();
    private:
        int _touched_millis = 0;
};

class Touch : public InputBase
{
    int DELETEME;
};

class OutputBase : public ModuleBase
{
    public:
        void off();
        void display(int);
};

class Poofer : public OutputBase
{
    int DELETEME;
};

class BodyLEDs : public OutputBase
{
    int DELETEME;
};
// XXX TODO XXX Add to the appropriate specific files
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

