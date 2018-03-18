// Three buttons doing some things. 
// Debra Lemak    10/14/17
// Update         02/13/18
// Wilkinson SXSW 03/09/18
// Portal Glitch  03/17/18

#include <FastLED.h>

// adjust for strand length
#define NUM_LEDS1  137
#define NUM_LEDS2  75
#define NUM_LEDS3  125

// depends on hardware wiring 
#define DATA_PIN1   9
#define DATA_PIN2  10
#define DATA_PIN3  11
#define BUTTON_PIN1 4
#define BUTTON_PIN2 3
#define BUTTON_PIN3 6

// should be outside pin range
#define BUTTON_NONE   31
#define BUTTON_AGAIN  63
#define BUTTON_HELD  127

#define HOLD_THRESHOLD 20 // probably should be increased (try 40?)

#define BRIGHTNESS  255
#define FRAMES_PER_SECOND 60

// LED color arrays
CRGB leds1[NUM_LEDS1];
CRGB leds2[NUM_LEDS2];
CRGB leds3[NUM_LEDS3];

// color selection globals
CRGBPalette16 gPal;  // setting here causes hang?  wtf?
uint8_t       gHue = 00; // rotating "base color" 

// replace global with arguments & return value, if possible
int   gLastButton       = BUTTON_NONE;
int   gCurrentPattern   = BUTTON_PIN1;
int   gButtonHoldCount  = 0;

/////////////////////////////////////////////////////////////////

void setup() {
  // set up palette global for color effects  
  gPal = CRGBPalette16(CRGB::Green, CRGB::Blue); 

  // set up the Buttons
  Serial.begin(9600);
  pinMode(BUTTON_PIN1, INPUT);
  pinMode(BUTTON_PIN2, INPUT);
  pinMode(BUTTON_PIN3, INPUT);

  // set up the LEDs
  FastLED.setBrightness(BRIGHTNESS); 
  FastLED.addLeds<WS2811, DATA_PIN1, RGB>(leds1, NUM_LEDS1);
  FastLED.addLeds<WS2811, DATA_PIN2, RGB>(leds2, NUM_LEDS2);
  FastLED.addLeds<WS2811, DATA_PIN3, RGB>(leds3, NUM_LEDS3);
  
  // clear and fill with something
  FastLED.clear();
  renderEffects(gCurrentPattern);
  FastLED.show();
}

/*
 * Main loop
 */
void loop() {
  int buttonPressed = checkInputs();

  if (buttonPressed == BUTTON_NONE) {
    // reset hold counter
    gButtonHoldCount = 0;     
  }
  else if (buttonPressed == gLastButton) {
    // multiple push, or being held?
    if (gButtonHoldCount == 0) {
      // already doing that?
       if (gCurrentPattern == gLastButton) {
        // then do the other thing!
        gCurrentPattern =  BUTTON_AGAIN;
      }
      else {
        // go back to original pattern for this button
        gCurrentPattern = buttonPressed;
      }
    }
    else {
      // button still held, but for how long?
      if (gButtonHoldCount >= HOLD_THRESHOLD) {
        gCurrentPattern = BUTTON_HELD;
      }
    }
    gButtonHoldCount++;
  }
  else {
    // new button pushed!
    gButtonHoldCount = 1;
    gLastButton     = buttonPressed;   
    gCurrentPattern = buttonPressed;
  }

  renderEffects(gCurrentPattern);
  FastLED.show();

  //FastLED.delay(1000/FRAMES_PER_SECOND); 
}

/**
 * Check our inputs and set the button state
 */
int checkInputs() {
  int buttonPressed = BUTTON_NONE;

  if (digitalRead(BUTTON_PIN2) == HIGH) {
    // middle button is smallest, so highest priority
    buttonPressed = BUTTON_PIN2;
  } 
  else if (digitalRead(BUTTON_PIN3) == HIGH) {
    // furthest right button is big, but on the right
    buttonPressed = BUTTON_PIN3;
  } 
  else if (digitalRead(BUTTON_PIN1) == HIGH) {
    // big left button hit often, so lowest priority
    buttonPressed = BUTTON_PIN1;
  }   
  else {
    // bail out early
    return BUTTON_NONE;
  }

//  Serial.println(buttonPressed);
  return buttonPressed;
}


/**
 * Handle the button state to render effects
 */
void renderEffects(int buttonPressed) {
    switch (buttonPressed) {
    case BUTTON_PIN1:
      bpm(leds1, NUM_LEDS1);
      bpm(leds2, NUM_LEDS2);
      bpm(leds3, NUM_LEDS3);
      break;
    case BUTTON_PIN2:
       juggle(leds1, NUM_LEDS1, 0, 32, 7);
      juggle(leds2, NUM_LEDS2, 0, 32, 7);
      juggle(leds3, NUM_LEDS3, 0, 32, 7); 
      break;
    case BUTTON_PIN3:
      rainbowWithGlitter(leds1, NUM_LEDS1); 
      rainbowWithGlitter(leds2, NUM_LEDS2);
      rainbowWithGlitter(leds3, NUM_LEDS3); 
      break;
    case BUTTON_AGAIN:
    case BUTTON_HELD:
      juggle(leds1, NUM_LEDS1,  64, 32, 7);
      juggle(leds2, NUM_LEDS2, 128, 32, 7);
      juggle(leds3, NUM_LEDS3, 192, 32, 7); 
      break;
     case BUTTON_NONE:
      Serial.println("No button pressed.");  
      break;
  }
}

/*
 * Efect #1
 */
void juggle(CRGB* strand, int numLights, byte baseHue, byte hueRotation, int beatIncrement) {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( strand, numLights, 20);
  byte dotHue = baseHue;
  for( int i = 0; i < 8; i++) {
    strand[beatsin16(i+beatIncrement, 0, numLights)] |= CHSV(dotHue, 200, 255);
    dotHue += hueRotation;
  }
}

/*
 * Efect #2
 */
void bpm(CRGB* strand, int numlights)
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 8;
  CRGBPalette16 palette = gPal;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < numlights; i++) { 
    strand[i] = ColorFromPalette(palette, gHue+(i*2.60), beat-gHue+(i*3.33));
  }
}


/*
 * Efect #3
 */
void rainbowWithGlitter(CRGB* leds, int NUM_LEDS) 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow(leds, NUM_LEDS);
  addGlitter(leds, NUM_LEDS, 80);
}

void rainbow(CRGB* leds, int NUM_LEDS) 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void addGlitter(CRGB* leds, int NUM_LEDS, fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}
