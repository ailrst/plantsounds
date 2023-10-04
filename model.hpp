#pragma once

// clang-format off
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_SAM)

#define STRINGT String

#else

#define STRINGT std::string

#include <string>

#endif // defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_SAM)
       
// Depends MsgPacketizer
#include <stdint.h>
// clang-format on 

typedef uint8_t chan_id;

struct Channel {
  chan_id id;
};


enum led_command_type : uint8_t {
  LED_START,
  LED_STOP,
  LED_MODE_PARTY,
  LED_MODE_RAINBOW,
  LED_MODE_COLOUR,
};

typedef uint8_t LED_COMMAND;

struct error {
  STRINGT message;
  MSGPACK_DEFINE(message);
};

struct inform_channel {
  chan_id channel;
  uint8_t light_pin;
  uint8_t sensor_pin;
  MSGPACK_DEFINE(channel, light_pin, sensor_pin);
};

struct led {
  chan_id channel;
  LED_COMMAND command;
  bool fade;
  MSGPACK_DEFINE(channel, command, fade);
};

struct touch_event {
  chan_id channel;
  MSGPACK_DEFINE(channel);
};



struct message {
  enum type : uint8_t { 
    ERROR = 0x10, 
    LED = 0x11, 
    INFORM_CHANNEL = 0x12, 
    TOUCH_EVENT = 0x13
  };

  typedef error ERROR_t;
  typedef led LED_t;
  typedef touch_event TOUCH_EVENT_t;
  typedef inform_channel INFORM_CHANNEL_t;
};


