
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


#define LED_PIN 5
#define NUM_LEDS 50 // TODO: this is a complete guess
#define BRIGHTNESS 64
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

// MsgPacketizer
#define MAX_CHANNELS 100

#define UPDATES_PER_SECOND 100
// ---------------

#define NUM_TOUCH_PINS 8

//char touch_state[NUM_TOUCH_PINS] = {}; // TODO: 7/8ths wasted space

byte button_pins[NUM_TOUCH_PINS] = {1,2,3,4,5,6,7,8};
Bounce buttons[NUM_TOUCH_PINS];

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
message::TOUCH_EVENT_t touch_state {0};

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
  
  // do this before starting serial because it prints some
  // debug stuff
  bool cap_found = cap.begin();

  Serial.begin(115200);
  if (!cap_found) {
    Serial.println("CAP1188 not found");
  }

  delay(3000); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
      .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  fill_solid(currentPalette, 16, CRGB::Black);
  fill_solid(blackPalette, 16, CRGB::Black);

  MsgPacketizer::publish(Serial, message::TOUCH_EVENT, touch_state)->setFrameRate(10);


  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
}

void handle_touches() {

  touch_state.state = cap.touched();

  if (touch_state.state == 0) {
    // No touch detected
    currentPalette = blackPalette;

  } else {

    currentPalette = PartyColors_p;
  }

  return;
  // bye

//  if (touched) {
//  
//    for (uint8_t i=0; i<8; i++) {
//      bool pin_touched = (touched & (1 << i));
//
//
//      if (pin_touched && !touch_state[i]) {
//        // activation  
//        MsgPacketizer::send(Serial, message::TOUCH_EVENT, message::TOUCH_EVENT_t {i});
//        // pubsub seems unneccessary between two things, and receipts do not seem to work
//        // so it just aggressively retries forever
//        //MsgPacketizer::send(Serial, message::TOUCH_EVENT, message::TOUCH_EVENT_t {i});
//      }
//      if (!pin_touched && touch_state[i]) {
//        // de-activation  
//      }
//
//      touch_state[i] = pin_touched;
//    }
//  }
//

}

void update_leds() {
  static uint8_t startIndex = 0;
  startIndex = startIndex + 3; /* motion speed */
  FillLEDsFromPaletteColors(startIndex);

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
  ChangePalettePeriodically() ;
}

void loop() {
static int loop = 0;
  //Serial.print("Loop "); Serial.println(loop++);
  handle_touches();
  update_leds();
  MsgPacketizer::update();
  MsgPacketizer::parse();
}

void clear_leds() {
  CRGB newcolor = CHSV(0, 0, 0);

  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = newcolor;
  }
}

void FillLEDsFromPaletteColors(uint8_t colorIndex) {
  uint8_t brightness = 255;

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
