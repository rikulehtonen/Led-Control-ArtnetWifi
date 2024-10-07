#ifndef MAPLEDS_H
#define MAPLEDS_H

#include <FastLED.h>

// LED settings
const int numLeds = 36; // Total number of LEDs
const int numLedsPerStrip = 18; // Number of LEDs per strip
const byte dataPin1 = 3; // Pin for strip1
const byte dataPin2 = 6; // Pin for strip2

// Arrays to store LED data
CRGB strip1[numLedsPerStrip];
int strip1_order[numLedsPerStrip];
CRGB strip2[numLedsPerStrip];
int strip2_order[numLedsPerStrip];

// Function to initialize the LED strips and their configurations
void initLeds() {
  // Initialize FastLED for both strips
  FastLED.addLeds<APA106, dataPin1, RGB>(strip1, numLedsPerStrip);  // LEDs 1-18 on pin 3
  FastLED.addLeds<APA106, dataPin2, RGB>(strip2, numLedsPerStrip);  // LEDs 19-36 on pin 6

  // Set up the order arrays for strip1 and strip2
  for(int i = 0; i < numLedsPerStrip; i++) {
    strip1_order[i] = numLedsPerStrip - 1 - i;
  }
  for(int i = 0; i < numLedsPerStrip; i++) {
    strip2_order[i] = i + numLedsPerStrip;
  }
}

// Function to map led_live to strip1 and strip2 according to the order arrays
void mapLeds(CRGB* led_live) {
  // Map the LEDs for strip1
  for (int i = 0; i < numLedsPerStrip; i++) {
    strip1[i] = led_live[strip1_order[i]];
  }
  
  // Map the LEDs for strip2
  for (int i = 0; i < numLedsPerStrip; i++) {
    strip2[i] = led_live[strip2_order[i]];
  }
}

#endif
