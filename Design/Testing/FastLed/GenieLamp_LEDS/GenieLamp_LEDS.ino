#include "FastLED.h"

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif
#define DATA_PIN 21
#define LED_LEFT_PIN 22
#define LED_LEFT_PIN 21

//#define CLK_PIN   4
#define LED_TYPE    WS2812
#define COLOR_ORDER RGB
#define NUM_LEDS    64
CRGB ledsL[NUM_LEDS];
CRGB ledsR[NUM_LEDS];
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

int fadeAmount = 5;  // Set the amount to fade I usually do 5, 10, 15, 20, 25 etc even up to 255.
uint8_t brightness = 0;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int timeTarget = 10000;
long timeCalled = 0;
long timeEnd = 0;
void setup() {
   Serial.begin(38400);
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,LED_LEFT_PIN,COLOR_ORDER>(ledsL, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,LED_LEFT_PIN,COLOR_ORDER>(ledsR, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  timeCalled = millis();
  timeEnd = timeCalled + timeTarget;
}


void loop(){
//  EVERY_N_MILLISECONDS( 20 ) { leds[i].fadeLightBy(brightness); } // slowly cycle the "base color" through the rainbow
redFade();
//sinelon();
  FastLED.show();
  FastLED.delay(1000/FRAMES_PER_SECOND); 
}
void redFade(){
  long brightCalc = map(millis(), timeCalled, timeEnd, 0, 255);
  for(int i = 0; i < NUM_LEDS; i++ )
   {
   ledsL[i].setRGB(0,255,0);  // setRGB functions works by setting
                             // (RED value 0-255, GREEN value 0-255, BLUE value 0-255)
                             // RED = setRGB(255,0,0)
                             // GREEN = setRGB(0,255,0)
   ledsR[i].setRGB(0,255,0);  // setRGB functions works by setting
                             // (RED value 0-255, GREEN value 0-255, BLUE value 0-255)
                             // RED = setRGB(255,0,0)
                             // GREEN = setRGB(0,255,0)
  }
    FastLED.setBrightness(brightCalc);
    Serial.print(timeCalled);
    Serial.print("  ");
    Serial.print(timeEnd);
    Serial.print("  ");
    // left off here.
   // Serial.print(millis());
    Serial.print("  ");
    Serial.println(brightCalc);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(ledsR, NUM_LEDS, 20);
    fadeToBlackBy(ledsL, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  ledsR[pos] += CHSV( gHue, 255, 192);
  ledsL[pos] += CHSV( gHue, 255, 192);
}
 /*
  brightness = brightness + fadeAmount;
  // reverse the direction of the fading at the ends of the fade:
  if(brightness == 0 || brightness == 255)
  {
    fadeAmount = -fadeAmount ;
  }
  */   

