// clang-format off
#include <memory>
#include <chrono>
#include <functional>
#include "ofMain.h"
#include "ofxMsgPacketizer.h"
#include "config.hpp"
#include "sounds.hpp"
#include "../../../model.hpp"
// clang-format on 


namespace plantmusic {

class sensor_manager {
  struct sensor_info {
    std::chrono::time_point<std::chrono::steady_clock> state_since {};
    bool state;
  };

  std::vector<sensor_info> sensor_state {};
  std::unordered_map<chan_id, bool> filtered_channel_state {};
  const std::unique_ptr<plantmusic::config> & config;
  public:

  sensor_manager(std::unique_ptr<plantmusic::config> & config) : config(config) {
    sensor_state.resize(config->sensor_channel_mapping.size());
  }

  auto get_now() {
    return std::chrono::steady_clock::now();
  }

  void update_sensor(const touch_event &t) {
    auto now = get_now();

    for (int i = 0; i < t.size(); i++) {
      if (sensor_state[i].state != t.is_active(i)) {
        sensor_state[i].state_since = now;
      }
      sensor_state[i].state = t.is_active(i);
    }
  }

  const std::unordered_map<chan_id, bool> &
    get_playing_channels() {
      for (const auto &[sensor, channel]: config->sensor_channel_mapping) {
        /*
         * Only change state of the channel if the time threshold has elapsed. 
         */
        auto threshold = sensor_state[sensor].state ? config->activation_threshold_ms : config->deactivation_threshold_ms;

        auto time_delta = get_now() - sensor_state[sensor].state_since;
        auto millis_delta = std::chrono::duration_cast<std::chrono::milliseconds>(time_delta).count();
        if (millis_delta > threshold) {
          filtered_channel_state[channel] = sensor_state[sensor].state;
        }
      }
      return filtered_channel_state;
    }

};

};
class ofApp : public ofBaseApp {

    std::unique_ptr<plantmusic::sensor_manager> sensors;
    std::unique_ptr<plantmusic::config> config;
    std::unique_ptr<plantmusic::player> player;

  ofSerial serial;

  std::stringstream echo_info;
  std::stringstream recv_info;


public:
  void setup() {
    config = std::make_unique<plantmusic::config>(plantmusic::config::default_config());
    player = std::make_unique<plantmusic::player>(config);
    sensors = std::make_unique<plantmusic::sensor_manager>(config);

    player->setup();
    ofSetVerticalSync(false);
    ofSetFrameRate(120);
    ofSetBackgroundColor(0);

    serial.setup("/dev/ttyUSB0", 115200);

    // publish packet (you can also send function returns)
    //MsgPacketizer::publish(serial, send_index, nested)->setFrameRate(60);

    // handle updated data from arduino
    MsgPacketizer::subscribe(serial, message::ERROR, [&](const message::ERROR_t &n) {
      ofLog()<< "message: error:" << n.message << std::endl;
    });

    MsgPacketizer::subscribe(serial, message::TOUCH_EVENT, [&](const message::TOUCH_EVENT_t &n) {

        sensors->update_sensor(n);
        static int x = 0;

        for (int i = 0; i < 8; i++) {
          if (n.is_active(i)) {
            player->play_sound(i);
            ofLog() << "touch event " << x++ << " on channel " << std::dec << i; 
          }
        }

    });


    // always called if packet has come regardless of index
    MsgPacketizer::subscribe(serial, [&](const uint8_t index,
                                         MsgPack::Unpacker &unpacker) {
      recv_info << "packet has come! index = 0x" << std::hex << (int)index;
      recv_info << ", arg size = " << std::dec << unpacker.size() << std::endl;
    });
  }


  void mousePressed(int x, int y, int button) {
      if (button == 0) {
          player->play_sound(0);
      }
      if (button == 1) {
          player->stop_sound(0);
          player->stop_sound(1);
      }
      if (button == 2) {
          player->play_sound(1);
      }
  }

  void update() {
    recv_info.str("");
    recv_info.clear();
    echo_info.str("");
    echo_info.clear();

    auto sens = sensors->get_playing_channels();
    for (auto &[channel, active]: sens) {
      if (active) {
        player->play_sound(channel);
      } else {
        player->stop_sound(channel);
      }

    }

    // must be called
    MsgPacketizer::update();
    MsgPacketizer::parse();
  }


  void draw() {
    ofDrawBitmapString("FPS : " + ofToString(ofGetFrameRate()), 20, 40);
    ofDrawBitmapString(recv_info.str(), 20, 80);
    ofDrawBitmapString(echo_info.str(), 20, 120);
  }
};

int main() {

  ofSetupOpenGL(480, 360, OF_WINDOW);
  ofRunApp(new ofApp());
}
