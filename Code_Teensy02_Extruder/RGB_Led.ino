#include <FastLED.h>
#define NUM_LEDS 1
#define DATA_PIN 23
CRGB leds[NUM_LEDS];

void RGB_setup()
{
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  delay(20);
  RGB_Off();
  RGB_Red();
  delay(1000);
  RGB_Green();
  delay(1000);
  RGB_Blue();
  delay(1000);
  RGB_Off();
  delay(10);
}


void RGB_Red()
{
  leds[0] = CRGB(255, 0, 0);
  FastLED.show();
}

void RGB_Lila()
{
  leds[0] = CRGB(255, 0, 255);
  FastLED.show();
}

void RGB_Green()
{
  leds[0] = CRGB(0, 255, 0);
  FastLED.show();
}

void RGB_Blue()
{
  leds[0] = CRGB(0, 0, 255);
  FastLED.show();
}

void RGB_Off()
{
  leds[0] = CRGB(0, 0, 0);
  FastLED.show();
}
