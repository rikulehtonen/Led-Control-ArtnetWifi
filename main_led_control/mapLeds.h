#ifndef MAPLEDS_H
#define MAPLEDS_H

#include <FastLED.h>

// LED settings
const int numLeds = 10; // Total number of LEDs
const int numLedsPerStrip = 10; // Number of LEDs per strip
const byte dataPin1 = 3; // Pin for strip1

// Arrays to store LED data
CRGB strip1[numLedsPerStrip];
int strip1_order[numLedsPerStrip];

// Function to initialize the LED strips and their configurations
void initLeds() {
  // Initialize FastLED for both strips
  FastLED.addLeds<WS2811, dataPin1, RGB>(strip1, numLedsPerStrip);  // LEDs 1-18 on pin 3

  for(int i = 0; i < numLedsPerStrip; i++) {
    strip1_order[i] = i;
  }
}

// Function to map led_live to strip1 and strip2 according to the order arrays
void mapLeds(CRGB* led_live) {
  // Map the LEDs for strip1
  for (int i = 0; i < numLedsPerStrip; i++) {
    strip1[i] = led_live[strip1_order[i]];
  }
  
}

#endif
