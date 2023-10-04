
#pragma once
#include <string>
#include <unordered_map>

namespace plantmusic {

struct config {
  std::string sounds_path = "data/sounds";
};

struct channel_mapping {
  std::unordered_map<int, std::string> channel_filename;
};

}; // namespace plantmusic
