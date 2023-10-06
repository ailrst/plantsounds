
#pragma once
#include "json.hpp"
#include <string>
#include <unordered_map>

namespace plantmusic {

struct config {
  std::string sounds_path = "data/sounds";
  int activation_threshold_ms = 300;
  int deactivation_threshold_ms = 500;
  std::vector<std::pair<int, std::string>> channel_sound_mapping;
  std::vector<std::pair<int, int>> sensor_channel_mapping;
  std::vector<std::string> sounds;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(config, sounds_path, activation_threshold_ms,
                                 deactivation_threshold_ms,
                                 channel_sound_mapping, sensor_channel_mapping,
                                 sounds);

  static config default_config() {
    config c;

    for (int i = 0; i < 8; i++) {
      c.sensor_channel_mapping.push_back({i, i});
    }

    return c;
  }
};

}; // namespace plantmusic
