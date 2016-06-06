// 4/9/2016 David Kent, Camp Lamp Retro Future
//    Fireseed Arts under stage tube lights

// setup for APA102 LED strip on Arduino MEGA_2560 using hardware SPI

#include "FastLED.h"

#define SERIAL_CMD

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

// BPM: 750-4000 is seizure, 120-140 for DJ
// BPM: 60 max for sinelon
#define BPM 60

#define DATA_PIN_1    51
#define CLK_PIN_1   52
#define DATA_PIN_2    44
#define CLK_PIN_2     45
#define LED_TYPE    APA102
#define COLOR_ORDER BGR
// define number of tubes and LEDs per tube
#define NUM_TUBES  20
#define NUM_LEDS_PER_TUBE  22
#define NUM_LEDS   NUM_TUBES * NUM_LEDS_PER_TUBE

// for multiple strips
#define NUM_STRIPS 2
#define NUM_LEDS_STRIP1 220
#define NUM_LEDS_STRIP2 NUM_LEDS - NUM_LEDS_STRIP1
//#define NUM_LEDS    300
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          255
#define FRAMES_PER_SECOND  60

int beats_per_minute = BPM;
int msec_per_beat;
int seizure_a=0, msec_per_beat_siezure;

// for serial communications
int incomingByte = 51;
int serial_ready = 0;

void setup() {

#if defined SERIAL_CMD
  Serial.begin(19200);
#endif

  // tell FastLED about the LED strip configuration
  //FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // strip 1
  FastLED.addLeds<LED_TYPE,DATA_PIN_1,CLK_PIN_1,COLOR_ORDER>(leds, 0, NUM_LEDS_STRIP1).setCorrection(TypicalLEDStrip);

  // strip 2
  FastLED.addLeds<LED_TYPE,DATA_PIN_2,CLK_PIN_2,COLOR_ORDER>(leds, NUM_LEDS_STRIP1, NUM_LEDS_STRIP2).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  delay(3000); // 3 second delay for recovery  

  // test code for color order - RED
  for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = CRGB::Red;
  }
  FastLED.show();  
  FastLED.delay(20); 

  // test code for color order - GREEN
  for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = CRGB::Green;
  }
  FastLED.show();  
  FastLED.delay(20); 
  
  // test code for color order - BLUE
  for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = CRGB::Blue;
  }
  FastLED.show();  
  FastLED.delay(20); 

  // test code for color order - BLUE
  for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();  

  
  /*
  // test code for color order - RED
  for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = CHSV(0, 255, 255);
  }
  FastLED.show();  
  FastLED.delay(500); 

  // test code for color order - GREEN
  for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = CHSV(96, 255, 255);
  }
  FastLED.show();  
  FastLED.delay(500); 

    // test code for color order - BLUE
  for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = CHSV(160, 255, 255);
  }
  FastLED.show();  
  FastLED.delay(500); 
*/

  msec_per_beat = (int)(1.0/(float)beats_per_minute * 60.0 * 1000.0);
  msec_per_beat_siezure = (int)(1.0/2000.0*60.0*1000.0);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { sinelon };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

//-------------------------------------------------------------------  
void loop()
{


#if defined SERIAL_CMD
  if (serial_ready == 1) { 
    if (Serial.available() > 0) {
      // read the incoming byte:
      incomingByte = Serial.read();

     // say what you got:
     Serial.println(incomingByte, DEC);
   }
  }
#endif

  if (incomingByte == 49) {
    // test code for color order - RED
    for (int i=0; i<NUM_LEDS; i++) {
      leds[i] = CRGB::Red;
    }
    FastLED.show();  
    FastLED.delay(msec_per_beat_siezure/2); 
  
    // test code for color order - BLUE
    for (int i=0; i<NUM_LEDS; i++) {
      leds[i] = CRGB::Blue;
    }
    FastLED.show();  
    FastLED.delay(msec_per_beat_siezure/2); 
  }

  if (incomingByte == 50) { 
    // Call the current pattern function once, updating the 'leds' array
    gPatterns[gCurrentPatternNumber]();

    // send the 'leds' array out to the actual LED strip
    FastLED.show();  
    // insert a delay to keep the framerate modest
    //FastLED.delay(msec_per_beat); 

    // do some periodic updates
    EVERY_N_MILLISECONDS( 2 ) { gHue++; } // slowly cycle the "base color" through the rainbow
    //EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  }


  if (incomingByte == 51) {
    fill_solid(leds, NUM_LEDS, CHSV( gHue, 200, 255));
/*
    for (int i=0; i<NUM_LEDS; i++) {
      leds[i] = CRGB(gHue);
    }
*/
    FastLED.show();  
//    FastLED.delay(msec_per_beat/4); 
    FastLED.delay(130); 

    for (int i=0; i<NUM_LEDS; i++) {
      leds[i] = CHSV( gHue-127, 200, 255);
    }
    FastLED.show();  
//    FastLED.delay(msec_per_beat/4); 
    FastLED.delay(130); 

//    if (gHue == 255) gHue = 0;
    gHue=gHue+10;
//    EVERY_N_MILLISECONDS( 2 ) { gHue++; } // slowly cycle the "base color" through the rainbow

  }


  if (incomingByte == 52) {
    for (int r=0; r<NUM_TUBES; r++) {
      fill_solid(leds+r*NUM_LEDS_PER_TUBE, NUM_LEDS_PER_TUBE, CHSV( gHue + random8(64), 200, 255));
    }
/*
    for (int i=0; i<NUM_LEDS; i++) {
      leds[i] = CRGB(gHue);
    }
*/
    FastLED.show();  
    FastLED.delay(70); 

    gHue=gHue+10;
  }


  if (incomingByte == 53) {
    for (int r=0; r<NUM_LEDS_PER_TUBE; r++) {
      fill_solid(leds+r*NUM_TUBES, NUM_TUBES, CHSV( gHue + random8(64), 200, 255));
    }
/*
    for (int i=0; i<NUM_LEDS; i++) {
      leds[i] = CRGB(gHue);
    }
*/
    FastLED.show();  
    FastLED.delay(50); 

    gHue=gHue+10;
  }


  if (incomingByte == 54) {
    // test code for color order - RED
    for (int i=0; i<NUM_LEDS; i++) {
      leds[i] = CHSV(gHue,255,255);
    }
    FastLED.show();  
    FastLED.delay(msec_per_beat_siezure/2); 
  
    // test code for color order - BLUE
    for (int i=0; i<NUM_LEDS; i++) {
      leds[i] = CHSV(0,0,0);
    }
    FastLED.show();  
    FastLED.delay(msec_per_beat_siezure/2); 
    gHue = gHue + random8(64);
  }

/*
  if (incomingByte == 55) {
    for (int t=0; t<NUM_TUBES; t++) {
      int i;
      for (; i<NUM_LEDS_PER_TUBE; i++) {
        leds[i] = CHSV(gHue,255,255);
      }
      gHue = gHue + random8(64);
    }
    FastLED.show();  
    FastLED.delay(msec_per_beat_siezure/2); 
  
    // test code for color order - BLUE
    for (int i=0; i<NUM_LEDS; i++) {
      leds[i] = CHSV(0,0,0);
    }
    FastLED.show();  
    FastLED.delay(msec_per_beat_siezure/2); 
    gHue = gHue + random8(64);
  }
*/


/*
  if (incomingByte == 54) {
    for (int r=0; r<NUM_LEDS_PER_TUBE; r++) {
//      fill_solid(leds+r*NUM_TUBES, 1, CHSV( gHue, 200, 255));
      for (int s=0; s<NUM_TUBES;) {
        leds[s+r*NUM_LEDS_PER_TUBE] = CHSV(gHue, 255, 0);
        leds[++s+r*NUM_LEDS_PER_TUBE] = CHSV( gHue, 200, 255);
      }
    }
    FastLED.show();  
    FastLED.delay(50); 

    gHue=gHue+10;
  }
*/

}


//--------------------------------------------------------------------
void serialEvent() {
#if defined SERIAL_CMD
  serial_ready = 1;
#endif
}



//--------------------------------------------------------------------
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))



//--------------------------------------------------------------------
void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  /*
  for (int q=0; q<NUM_TUBES; q++) {
    fill_rainbow( leds+(NUM_LEDS_PER_TUBE*q), NUM_LEDS_PER_TUBE, gHue+q, 1);
  }
  */
  fill_rainbow( leds, NUM_LEDS, gHue, 1);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(60,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 255);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = BPM;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}



