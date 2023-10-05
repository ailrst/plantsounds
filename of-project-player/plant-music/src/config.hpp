
#pragma once
#include "json.hpp"
#include <string>
#include <unordered_map>

namespace plantmusic {

struct config {
  std::string sounds_path = "data/sounds";
  int activation_threshold_ms = 300;
  int deactivation_threshold_ms = 500;
  std::unordered_map<int, std::string> channel_sound_mapping;
  std::unordered_map<int, int> sensor_channel_mapping;

  static config default_config() {
    config c;

    for (int i = 0; i < 8; i++) {
      c.sensor_channel_mapping[i] = i;
    }

    return c;
  }
};

}; // namespace plantmusic
