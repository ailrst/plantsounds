
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

#include <Adafruit_CAP1188.h>


#include <MsgPacketizer.h>

#include <FastLED.h>

#include "model.hpp"

#define NUM_LEDS 70 // TODO: this is a complete guess
#define LED_PIN 5
#define BRIGHTNESS 64
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
//message::LIGHTING_UPDATE_t brightnesses;

/*

This program is right up at the limit of memory for operation. 

Needs at least 700bytes for local variables for it to function.

*/


// MsgPacketizer
#define MAX_CHANNELS 1

#define UPDATES_PER_SECOND 14
// ---------------

#define NUM_TOUCH_PINS 8

//char touch_state[NUM_TOUCH_PINS] = {}; // TODO: 7/8ths wasted space
message::LED_UPDATE_t led_update;

CRGBPalette16 currentPalette;

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

const uint8_t led_index = message::LED_UPDATE;
const uint8_t touch_index = message::TOUCH_EVENT;
const uint8_t error_index = message::ERROR;

void setup() {
  //Serial.println("CAP1188 test!");

  // Initialize the sensor, if using i2c you can pass in the i2c address
  
  // do this before starting serial because it prints some
  // debug stuff
  //bool cap_found = cap.begin();
  if (!cap.begin()) {
    Serial.println("CAP1188 not found");
    while (1);
  }


  Serial.begin(115200);
  
  delay(3000); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
      .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  fill_solid(currentPalette, 16, CRGB::Red);


 // MsgPacketizer::subscribe(Serial, lighting_index, brightnesses);
//  MsgPacketizer::subscribe(Serial, led_index, 
 //   led_update);
  //      fill_solid(currentPalette, 16, CRGB::Blue);
  //    MsgPacketizer::send(Serial, error_index, message::ERROR_t {"Got message"});

  MsgPacketizer::publish(Serial, touch_index, touch_state)->setFrameRate(3);



}

void handle_touches() {

  touch_state.state = cap.touched();
 

  if (touch_state.state) {
    fill_solid(currentPalette, 16, CRGB::White);
  } else {
    fill_solid(currentPalette, 16, CRGB::Black);
  }


}

void update_leds() {
  //static uint8_t startIndex = 0;
  //startIndex = startIndex + 3; /* motion speed */
//  fill_solid(currentPalette, 16, CHSV(led_update.h, led_update.s, led_update.v));

  FillLEDsFromPaletteColors(0);

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
  //ChangePalettePeriodically() ;
}

void loop() {
//Serial.print("Loop "); Serial.println(loop++);


  //fill_solid(currentPalette, 16, CRGB::Blue);
  handle_touches();

  update_leds();

  MsgPacketizer::parse();
  MsgPacketizer::update();
}

void FillLEDsFromPaletteColors(uint8_t colorIndex) {
  uint8_t brightness = 255;

  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness,
                               LINEARBLEND);
    colorIndex += 3;
  }
}
