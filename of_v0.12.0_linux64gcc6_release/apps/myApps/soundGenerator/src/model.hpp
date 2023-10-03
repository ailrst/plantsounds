

#include <stdint.h>

typedef uint8_t chan_id;

struct channel {
  chan_id id;
};

enum message_type : uint8_t { ERROR, LED, INFORM_CHANNEL, TOUCH_EVENT };

enum led_command_type : uint8_t {
  LED_START,
  LED_STOP,
  LED_MODE_PARTY,
  LED_MODE_RAINBOW,
  LED_MODE_COLOUR,
};

struct error {
  char *message;
};

struct inform_channel {
  chan_id channel;
  uint8_t light_pin;
  uint8_t sensor_pin;
};

struct led {
  led_command_type command;
  chan_id channel;
  bool fade;
};

struct touch_event {
  chan_id channel;
};

struct command {};
