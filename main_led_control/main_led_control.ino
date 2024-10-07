#include <ArtnetWifi.h>
#include <Arduino.h>
#include <FastLED.h>
#include "arduino_secrets.h"
#include "mapLeds.h"

// Wifi settings
const char ssid[] = SECRET_SSID;
const char password[] = SECRET_PASS;

// LED settings
const int numberOfChannels = numLeds * 3; // Total number of channels you want to receive (1 led = 3 channels)

CRGB led_live[numLeds];  // Single array to handle all LEDs
CRGB led_buffer[numLeds];

// Art-Net settings
ArtnetWifi artnet;
const int startUniverse = 2; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.

// Check if we got all universes
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;

unsigned long previousMillisFade = 0;
unsigned long previousMillisRotate = 0;
long intervalFade = 200; // interval for rotation (1 second)
long intervalRotate = 2000;
short amountOfP2 = 50;
short updateLayer = 255;

// connect to wifi – returns true if successful or false if not
bool ConnectWifi(void)
{
  bool state = true;
  int i = 0;
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    if (i > 20)
    {
      state = false;
      break;
    }
    i++;
  }
  if (state)
  {
    led_live[0] = CRGB(0,255,0);
    FastLED.show();
    delay(2000);
  }
  else
  {
    led_live[0] = CRGB(0,255,0);
    FastLED.show();
    delay(2000);
  }

  return state;
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  sendFrame = 1;
  
  // set brightness of the whole strip
  if (universe == 15)
  {
    FastLED.setBrightness(data[0]);
    FastLED.show();
  }

  // range check
  if (universe < startUniverse)
  {
    return;
  }
  uint8_t index = universe - startUniverse;
  if (index >= maxUniverses)
  {
    return;
  }

  // Store which universe has got in
  universesReceived[index] = true;
  for (int i = 0 ; i < maxUniverses ; i++)
  {
    if (!universesReceived[i])
    {
      sendFrame = 0;
      break;
    }
  }

  // read universe and put into the right part of the display buffer
  for (int i = 0; i < length / 3; i++)
  {
    int led = i + (index * 170);
    if (updateLayer && led < numLeds)
    {
      led_buffer[led] = CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    else if (led == numLeds) 
    {
      updateLayer = data[i * 3];
      intervalFade = data[i * 3 + 1] * 25;
      intervalRotate = data[i * 3 + 2] * 25;
    }    
  }

  if (sendFrame)
  {
    // Reset universeReceived to 0
    memset(universesReceived, 0, maxUniverses);
  }
}

void setFade() {
  unsigned long currentMillisFade = millis();
  if (currentMillisFade - previousMillisFade >= intervalFade) 
  {
    previousMillisFade = currentMillisFade;
    for (int i = 0 ; i < numLeds ; i++)
      {
        led_live[i] = blend(led_live[i], led_buffer[i], amountOfP2);
      }
  }
}

void setRotate() {
  unsigned long currentMillisRotate = millis();
  if (intervalRotate && currentMillisRotate - previousMillisRotate >= intervalRotate) {
    previousMillisRotate = currentMillisRotate;

    CRGB temp = led_buffer[numLeds - 1];
    for (int i = numLeds - 1; i > 0; i--) {
      led_buffer[i] = led_buffer[i - 1];
    }
    led_buffer[0] = temp;
  }
}

// Updated animate function
void animate() {
  if (!sendFrame) {
    return;
  }
  setFade();
  setRotate();

  // Use the mapLeds function from the header
  mapLeds(led_live);

  FastLED.show();
}

void setup() {
  Serial.begin(9600);
  // Initialize the LEDs
  initLeds();
  ConnectWifi();
  artnet.begin();

  // Assign the initial colors to the LEDs
  for (int i = 0; i < numLeds; ++i) {
    if (i % 2 == 0) { led_buffer[i] = CRGB(250, 90, 0); } else { led_buffer[i] = CRGB(62.5, 22.5, 0); }
  }
  
  memset(universesReceived, 0, maxUniverses);
  
  // Set the callback for Art-Net DMX frames
  artnet.setArtDmxCallback(onDmxFrame);
}

void loop()
{
  // we call the read function inside the loop
  artnet.read();
  animate();
}
