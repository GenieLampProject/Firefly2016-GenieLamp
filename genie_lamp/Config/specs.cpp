//#include <XXX.h>


//const int foo = bar;

class PatternConfig
{
    private:

    public:
        const int num;
        PatternConfig* child;

        void print() const
        {
            Keyboard.print("num=");
            Keyboard.println( num );
        }
};


//void setup() {}

//void loop() {}

