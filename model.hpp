#pragma once

#undef PACKETIZER_USE_CRC_AS_DEFAULT
#include <MsgPack.h>

// max publishing elemtnt size in one destination
// #define MSGPACKETIZER_MAX_PUBLISH_ELEMENT_SIZE 64
//// max destinations to publish
// #define MSGPACKETIZER_MAX_PUBLISH_DESTINATION_SIZE 1
//
//// msgpack serialized binary size
// #define MSGPACK_MAX_PACKET_BYTE_SIZE 96
//// max size of MsgPack::arr_t
// #define MSGPACK_MAX_ARRAY_SIZE 64
//// max size of MsgPack::map_t
// #define MSGPACK_MAX_MAP_SIZE 64
//// msgpack objects size in one packet
// #define MSGPACK_MAX_OBJECT_SIZE 64
//
//// max number of decoded packet queues
// #define PACKETIZER_MAX_PACKET_QUEUE_SIZE 2
//// max data bytes in packet
// #define PACKETIZER_MAX_PACKET_BINARY_SIZE 96
//// max number of callback for one stream
// #define PACKETIZER_MAX_CALLBACK_QUEUE_SIZE 3
//// max number of streams
// #define PACKETIZER_MAX_STREAM_MAP_SIZE 2
//
// clang-format off
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_SAM)

#define STRINGT String

#else

#define STRINGT std::string

#include <string>
#include <stdint.h>

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

struct led_update {
  uint8_t led;
  MSGPACK_DEFINE(led);
  //MsgPack::bin_t<uint8_t> lights;
};


struct touch_event {
  uint8_t state;
  MSGPACK_DEFINE(state);


  int size() const {
    return 8;
  }

  bool is_active(uint8_t pin) const {
    return (1 << pin) & state;
  }

};


struct lighting_update {
  chan_id channel;
  MsgPack::bin_t<uint8_t> lights;
  MSGPACK_DEFINE(channel, lights);
};


namespace message {
  enum type : uint8_t { 
    ERROR = 0x10, 
    LED = 0x21, 
    INFORM_CHANNEL = 0x32, 
    TOUCH_EVENT = 0x43,
    LIGHTING_UPDATE = 0x54,
    LED_UPDATE = 0x64
  };

  typedef error ERROR_t;
  typedef led LED_t;
  typedef touch_event TOUCH_EVENT_t;
  typedef inform_channel INFORM_CHANNEL_t;
  typedef lighting_update LIGHTING_UPDATE_t;
  typedef led_update LED_UPDATE_t;
};


