// Three buttons doing some things. 
// Debra Lemak 10/14/17
// Update      02/13/18
// Wilkinson   03/09/18

#include <FastLED.h>

#define NUM_LEDS1  75
#define NUM_LEDS2 118
#define NUM_LEDS3 144

#define DATA_PIN1   9
#define DATA_PIN2  10
#define DATA_PIN3  11

#define BUTTON_PIN1 4
#define BUTTON_PIN2 3
#define BUTTON_PIN3 6
#define BUTTON_NONE 0

#define BRIGHTNESS  255
#define FRAMES_PER_SECOND 60

// LED color arrays
CRGB leds1[NUM_LEDS1];
CRGB leds2[NUM_LEDS2];
CRGB leds3[NUM_LEDS3];

// color selection globals
CRGBPalette16 gPal;  // setting here causes hang?  wtf?
uint8_t       gHue = 00; // rotating "base color" 

// replace global with return value
// int buttonPressed = 0;

//CRGBPalette16 currentPalette(ForestColors_p);
//CRGBPalette16 targetPalette(OceanColors_p);


///////Pulling in new color sequence from Andrew Tuline, Title: inoise8_pal_demo.ino

//uint16_t scale = 30;          // Wouldn't recommend changing this on the fly, or the animation will be really blocky.
uint8_t maxChanges = 48;      // Value for blending between palettes.
 

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
  renderEffects(BUTTON_PIN1); // pretend a button was pushed
  FastLED.show();
}

int gLastButton;
/*
 * Main loop
 */
void loop() {
  int buttonPressed = checkInputs();
  
  if (buttonPressed != BUTTON_NONE) {
    gLastButton = buttonPressed;   
  }
  renderEffects(gLastButton);
  FastLED.show();


  
  //FastLED.delay(1000/FRAMES_PER_SECOND); 
}

/**
 * Check our inputs and set the button state
 */
int checkInputs() {
 int buttonPressed = BUTTON_NONE;
 
 if (digitalRead(BUTTON_PIN1) == HIGH) {
    // highest priority button
    buttonPressed = BUTTON_PIN1;
  } 
  else if (digitalRead(BUTTON_PIN2) == HIGH) {
    buttonPressed = BUTTON_PIN2;
  } 
  else if (digitalRead(BUTTON_PIN3) == HIGH) {
    // lowest priority button
    buttonPressed = BUTTON_PIN3;
  } 
  else {
    // bail out early
    return BUTTON_NONE;
  }

  Serial.println(buttonPressed);
  return buttonPressed;
}


/**
 * Handle the button state to render effects
 */
void renderEffects(int buttonPressed) {
    switch (buttonPressed) {
    case BUTTON_PIN1:
     Serial.println("Button 1 pressed.");  
      bpm(leds1, NUM_LEDS1);
      bpm(leds2, NUM_LEDS2);
      bpm(leds3, NUM_LEDS3);
      break;
    case BUTTON_PIN2:
      Serial.println("Button 2 pressed.");  
      juggle(leds1, NUM_LEDS1);
      juggle(leds2, NUM_LEDS2);
      juggle(leds3, NUM_LEDS3); 
      break;
    case BUTTON_PIN3:
      Serial.println("Button 3 pressed."); 
      rainbowWithGlitter(leds1, NUM_LEDS1); 
      rainbowWithGlitter(leds2, NUM_LEDS2);
      rainbowWithGlitter(leds3, NUM_LEDS3); 
      break;
    case BUTTON_NONE:
      Serial.println("No button pressed.");  
      break;
  }
}

/*
 * Efect #1
 */
void juggle(CRGB* strand, int numlights) {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( strand, numlights, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    strand[beatsin16(i+7,0,numlights)] |= CHSV(dothue, 200, 255);
    dothue += 32;
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
