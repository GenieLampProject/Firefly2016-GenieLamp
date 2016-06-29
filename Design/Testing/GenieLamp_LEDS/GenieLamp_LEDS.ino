#include "FastLED.h"
//TODO - include a timer for async usage.
//changed#include "timer.h"
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

#define LEFT_PIN    22
#define RIGHT_PIN    21
#define LED_TYPE    WS2812
#define COLOR_ORDER RGB
#define LEFT_NUM_LEDS    200
#define RIGHT_NUM_LEDS    200
CRGB left[LEFT_NUM_LEDS];
CRGB right[RIGHT_NUM_LEDS];

#define BRIGHTNESS          50
#define FRAMES_PER_SECOND  120

int fadeAmount = 5;  // Set the amount to fade I usually do 5, 10, 15, 20, 25 etc even up to 255.
uint8_t brightness = 0;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int timeTarget = 10000;
long timeCalled = 0;
long timeEnd = 0;
void setup() {
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,LEFT_PIN,COLOR_ORDER>(left, LEFT_NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,RIGHT_PIN,COLOR_ORDER>(right, RIGHT_NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  timeCalled = millis();
  timeEnd = timeCalled + timeTarget;
}
bool redFadeUp(){
  long brightCalc = constrain(map(millis(), timeCalled, timeEnd, 0, 255), 0, 255);
  for(int i = 0; i < LEFT_NUM_LEDS; i++ )
   {
   left[i].setRGB(0,255,0);  // setRGB functions works by setting
    right[i].setRGB(0,255,0);
  }
    FastLED.setBrightness(brightCalc);
    if (brightCalc == 255) {
      return true;
    } else {
       Serial.print(timeCalled);
    Serial.print("  ");
    Serial.print(timeEnd);
    Serial.print("  ");
    // left off here.
   // Serial.print(millis());
    Serial.print("  ");
    Serial.println(brightCalc);
    return false;
    }
}
void loop(){
//  EVERY_N_MILLISECONDS( 20 ) { leds[i].fadeLightBy(brightness); } // slowly cycle the "base color" through the rainbow
bool fadeDone;
  while (true) {
    fadeDone = redFadeUp();
    if (!fadeDone) {
      redFadeUp();
        FastLED.show();
  FastLED.delay(1000/FRAMES_PER_SECOND); 
    } else {

    }
}
}
