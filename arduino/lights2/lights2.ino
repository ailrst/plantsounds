
/***************************************************
  This is a library for the CAP1188 I2C/SPI 8-chan Capacitive Sensor

  Designed specifically to work with the CAP1188 sensor from Adafruit
  ----> https://www.adafruit.com/products/1602

  These sensors use I2C/SPI to communicate, 2+ pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CAP1188.h>

//#include <MsgPacketizer.h>

#include <FastLED.h>

#include <Bounce2.h>

//#include "model.hpp"

#define MIDI_SERIAL_RATE 31250


#define sound_expiry 9000

#define LED_PIN 6
#define NUM_LEDS 50 // TODO: this is a complete guess
#define BRIGHTNESS 64
#define LED_TYPE WS2812
#define COLOR_ORDER GRB

#define BUTTON_THRESHOLD 200

// led
#define UPDATES_PER_SECOND 10


#define NUM_TOUCH_PINS 1

void FillLEDsFromPaletteColors() ;

typedef struct _MIDImessage { //build structure for Note and Control MIDImessages
  unsigned int type;
  int value;
  int velocity;
  long duration;
  long period;
  int channel;
} 
MIDImessage;
MIDImessage noteArray[NUM_TOUCH_PINS]; 
MIDImessage controlMessage; 

void midiSerial(int type, int channel, int number, int velocity);


//char touch_state[NUM_TOUCH_PINS] = {}; // TODO: 7/8ths wasted space

byte button_pins[NUM_TOUCH_PINS] = {3};
Bounce buttons[NUM_TOUCH_PINS];
byte button_led_mapping[NUM_TOUCH_PINS] = {0};

#define num_led_segments 1
byte num_leds_in_segment[num_led_segments] = {NUM_LEDS};
byte led_segment_motion_index[num_led_segments] = {};
byte led_segment_begin[num_led_segments] = {};
CRGB leds[NUM_LEDS];

CRGBPalette16 led_palettes[num_led_segments] = {};



CRGBPalette16 currentPalette;
CRGBPalette16 blackPalette;

TBlendType currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;



// For I2C, connect SDA to your Arduino's SDA pin, SCL to SCL pin
// On UNO/Duemilanove/etc, SDA == Analog 4, SCL == Analog 5
// On Leonardo/Micro, SDA == Digital 2, SCL == Digital 3
// On Mega/ADK/Due, SDA == Digital 20, SCL == Digital 21

// Use I2C, no reset pin!
Adafruit_CAP1188 cap = Adafruit_CAP1188();
//message::TOUCH_EVENT_t touch_state {0};

// Or...Use I2C, with reset pin
// Adafruit_CAP1188 cap = Adafruit_CAP1188(CAP1188_RESET);

// Or... Hardware SPI, CS pin & reset pin
// Adafruit_CAP1188 cap = Adafruit_CAP1188(CAP1188_CS, CAP1188_RESET);

// Or.. Software SPI: clock, miso, mosi, cs, reset
// Adafruit_CAP1188 cap = Adafruit_CAP1188(CAP1188_CLK, CAP1188_MISO,
// CAP1188_MOSI, CAP1188_CS, CAP1188_RESET);

void setup() {
  //Serial.println("CAP1188 test!");

  // Initialize the sensor, if using i2c you can pass in the i2c address
  


  //Serial.begin(9600);
  Serial.begin(MIDI_SERIAL_RATE);

  for (int i = 0; i < NUM_TOUCH_PINS; i++) {
    buttons[i].attach(button_pins[i], INPUT_PULLUP);
    buttons[i].interval(25);
  }

  delay(3000); // power-up safety delay

  int running_led_offset = 0;

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, 
      NUM_LEDS).setCorrection(TypicalLEDStrip);


  FastLED.setBrightness(BRIGHTNESS);


  fill_solid(currentPalette, 16, CRGB::Black);
  fill_solid(blackPalette, 16, CRGB::Black);

  for (int i = 0; i < num_led_segments; i++) {
    led_palettes[i] = blackPalette;
  }

  //MsgPacketizer::publish(Serial, message::TOUCH_EVENT, touch_state)->setFrameRate(10);

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
}

void handle_touches() {

  static long became_on = millis();
  static bool state = false;

  for (int i = 0; i < NUM_TOUCH_PINS; i++) {
    buttons[i].update();

    if (buttons[i].fell()) {
        state = true;
        midiSerial(144, i, 440, 255); 
        became_on = millis();
        int led = button_led_mapping[i]; 
        led_palettes[led] = PartyColors_p;
        currentPalette = PartyColors_p;
    } 

    if (buttons[i].rose()) {
        state = false;
        midiSerial(144, i, 440, 0); 
        int led = button_led_mapping[i]; 
        currentPalette = blackPalette;
    }

    if (state && (millis()  - became_on) > sound_expiry) {
      // stop  at end of note
      midiSerial(144, i, 440, 0); 
      state = false;
      int led = button_led_mapping[i]; 
      led_palettes[led] = blackPalette;
      currentPalette = blackPalette;
    }


  }
    FillLEDsFromPaletteColors();
}


void midiSerial(int type, int channel, int number, int velocity) {

    //  Note type = 144
    //  Control type = 176  
    // remove MSBs on data
    number &= 0x7F;  //number
    velocity &= 0x7F;  //velocity
    
    byte statusbyte = (type | ((channel-1) & 0x0F));
    
    Serial.write(statusbyte);
    Serial.write(number);
    Serial.write(velocity);
  sei(); //enable interrupts
}


void update_leds() {

  FillLEDsFromPaletteColors();
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void loop() {
  //Serial.print("Loop "); Serial.println(loop++);
  handle_touches();
  update_leds();
  //MsgPacketizer::update();
  //MsgPacketizer::parse();
}

void clear_leds() {
  CRGB newcolor = CHSV(0, 0, 0);

  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = newcolor;
  }
}


void FillLEDsFromPaletteColors(
CRGB *begin, CRGB*end, 
CRGBPalette16  &palette,
uint8_t colorIndex) {
  uint8_t brightness = 255;

  for (CRGB* led = begin; led != end+1; led++) {
    *led = ColorFromPalette(palette, colorIndex, brightness,
                               currentBlending);
    colorIndex += 3;
  }
}

void FillLEDsFromPaletteColors() {
  uint8_t brightness = 255;
  static int colorIndex = 0;
  colorIndex += 1;

  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness,
                               currentBlending);
    colorIndex += 3;
  }
}

// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p,
// RainbowStripeColors_p, OceanColors_p, CloudColors_p, LavaColors_p,
// ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can
// write code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically() {
  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;

  if (lastSecond != secondHand) {
    //Serial.println("CHANGE PALETTE WOO\n");
    lastSecond = secondHand;
    if (secondHand == 0) {
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 10) {
      currentPalette = RainbowStripeColors_p;
      currentBlending = NOBLEND;
    }
    if (secondHand == 15) {
      currentPalette = RainbowStripeColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 20) {
      SetupPurpleAndGreenPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 25) {
      SetupTotallyRandomPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 30) {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = NOBLEND;
    }
    if (secondHand == 35) {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 40) {
      currentPalette = CloudColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 45) {
      currentPalette = PartyColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 50) {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = NOBLEND;
    }
    if (secondHand == 55) {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = LINEARBLEND;
    }
  }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette() {
  for (int i = 0; i < 16; ++i) {
    currentPalette[i] = CHSV(random8(), 255, random8());
  }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette() {
  // 'black out' all 16 palette entries...
  fill_solid(currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette() {
  CRGB purple = CHSV(HUE_PURPLE, 255, 255);
  CRGB green = CHSV(HUE_GREEN, 255, 255);
  CRGB black = CRGB::Black;

  currentPalette =
      CRGBPalette16(green, green, black, black, purple, purple, black, black,
                    green, green, black, black, purple, purple, black, black);
}

// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM = {
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue, CRGB::Black,

    CRGB::Red,  CRGB::Gray,  CRGB::Blue,  CRGB::Black,

    CRGB::Red,  CRGB::Red,   CRGB::Gray,  CRGB::Gray,
    CRGB::Blue, CRGB::Blue,  CRGB::Black, CRGB::Black};
