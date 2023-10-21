// clang-format off
#include "cxxopts.hpp"
#include <memory>
#include <tuple>
#include <chrono>
#include <functional>
#include <fstream>
#include <sstream>
#include "ofMain.h"
#include "ofxMsgPacketizer.h"
#include "config.hpp"
#include "sounds.hpp"
#include "../../../model.hpp"
#include "waveform-generator.hpp"
// clang-format on 

using json = nlohmann::json;

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

  std::tuple<const std::string, const std::string> log_state() {
    auto now = get_now();
    std::stringstream sensor_log;
    std::stringstream state_log;
    sensor_log << std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    state_log << std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    for (int i = 0; i < sensor_state.size(); i++) {
      sensor_log << ", " << sensor_state[i].state; 
      state_log << ", " << filtered_channel_state[i]; 
    }
    ofLog() << "sensor: " << sensor_log.str();
    ofLog() << "smooth: " << state_log.str();
    return {sensor_log.str(), state_log.str()};
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

  std::filesystem::file_time_type last_config_modification {};
  const std::string config_path = "data/config.json"; 


  void check_load_config() {
    std::ifstream f(config_path);
    if (!f) {
      return;
    }

    auto t = std::filesystem::directory_entry(config_path).last_write_time();
    if (last_config_modification == t) {
      return;
    }

    last_config_modification = t;
      
    json j;
    f >> j;
    plantmusic::config c = j;
    config = std::make_unique<plantmusic::config>(c);
    player->reload_config();

    ofLog()<< "reloaded config.";
  }

  void write_back_config() {
    std::ofstream o(config_path);
    o << json(*config).dump(2) << std::endl;
    last_config_modification = std::filesystem::directory_entry(config_path).last_write_time();
  }

  std::string serial_port;
public:

  ofApp(std::string port) : serial_port(port) {
  }

  void setup() {

  std::ifstream f(config_path);
  config = std::make_unique<plantmusic::config>(plantmusic::config::default_config());
  if (f) {
    json j;
    f >> j;
    plantmusic::config c = j;
    config = std::make_unique<plantmusic::config>(c);
  } else {
    write_back_config();
  }

    player = std::make_unique<plantmusic::player>(config);


    sensors = std::make_unique<plantmusic::sensor_manager>(config);

    player->setup();

    // update config
    write_back_config();

    ofSetVerticalSync(false);
    ofSetFrameRate(120);
    ofSetBackgroundColor(0);

    serial.setup(serial_port, 115200);

    // publish packet (you can also send function returns)
    //MsgPacketizer::publish(serial, send_index, nested)->setFrameRate(60);

    // handle updated data from arduino
    MsgPacketizer::subscribe(serial, message::ERROR, [&](const message::ERROR_t &n) {
      ofLog()<< "message: error:" << n.message;
    });

    MsgPacketizer::subscribe(serial, message::TOUCH_EVENT, [&](const message::TOUCH_EVENT_t &n) {

        sensors->update_sensor(n);

        for (int i = 0; i < 8; i++) {
          if (n.is_active(i)) {
            player->play_sound(i);
            //ofLog() << "touch event " << x++ << " on channel " << std::dec << i; 
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
    check_load_config();
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

    std::string sens, smoothed;
    std::tie (sens, smoothed) = sensors->log_state();

    //ofDrawBitmapString("FPS : " + ofToString(ofGetFrameRate()), 20, 40);
    ofDrawBitmapString(recv_info.str(), 20, 80);
    ofDrawBitmapString(echo_info.str(), 20, 120);

    ofDrawBitmapString(sens, 20, 150);
    ofDrawBitmapString(smoothed, 20, 180);

    int i = 180;
    ofDrawBitmapString("Sounds playing: ", 20, (i += 30));
    i += 10;
    for (const auto&[chan, sound] : player->sounds_mapping) {
      std::stringstream s;
      if (sound.player.isPlaying()) {
        s << "  channel " << chan << " " << sound.filename;
        ofDrawBitmapString(s.str(), 20, (i += 15));
      }

    }

  }
};

int main(int argc, char **argv) {

    cxxopts::Options options("plant-music", "Sound player");

    options.add_options()
        ("p,port", "Ardino serial port", cxxopts::value<std::string>()->default_value("/dev/ttyUSB0"))
        ;



    auto result = options.parse(argc, argv);
    auto port = result["port"].as<std::string>();

  ofSetupOpenGL(480, 360, OF_WINDOW);
  ofRunApp(new ofApp(port));
}
