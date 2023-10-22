
#include "../../../model.hpp"
#include "ofxMsgPacketizer.h"
#include "sounds.hpp"
#include "waveform-generator.hpp"
#include <memory>
#include <optional>
#include <type_traits>

const uint8_t led_update_index = 0x64;

namespace plantmusic {
class visualiser {

private:
  std::vector<uint8_t> *brightness_val = nullptr;
  const sound &snd;
  ofSerial &serial;

public:
  message::LIGHTING_UPDATE_t brightness{0, {}};
  visualiser(visualiser &v) = delete;
  visualiser(const visualiser &v) = delete;

  visualiser(ofSerial &serial, const chan_id channel, const sound &sound)
      : snd(sound), serial(serial) {
    brightness.channel = channel;
    brightness.lights = {};

    if (!snd.lights) {
      ofLog(OF_LOG_WARNING) << "Sound '" << sound.filename
                            << "' does not have lightpattern attached.";
      return;
    }
  };

  static std::unique_ptr<visualiser> create(ofSerial &serial, chan_id channel,
                                            const sound &sound) noexcept {
    if (!sound.lights) {
      return nullptr;
    }

    return std::make_unique<visualiser>(serial, channel, sound);
  }

  void update() {
    if (!snd.lights) {
      ofLog() << "No lights for sound :(";
      return;
    }

    if (!snd.player.isPlaying()) {
      ofLog() << "sound not playing";
      return;
    }

    ofLog() << "Get keyframe";
    auto new_val =
        snd.lights->get_keyframe_for_time_millis(snd.player.getPositionMS());

    if (!new_val.size()) {
      ofLog(OF_LOG_WARNING) << "Failed to get lighting keyframe!!";
      return;
    }

    // brightness.lights.resize(new_val.size());

    if (&new_val != brightness_val) {
      brightness_val = &new_val;
      // began new keyframe
      this->brightness.lights.resize(new_val.size());
      for (int i = 0; i < new_val.size(); i++) {
        // this->brightness.lights[i] = new_val[i];
      }
      ofLog() << "Send update\n";
      MsgPacketizer::send(serial, led_update_index, message::LED_UPDATE_t(100));

      // MsgPacketizer::send(serial, message::LIGHTING_UPDATE, brightness);
    }
  }
};

} // namespace plantmusic
