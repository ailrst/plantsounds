
#pragma once

// clang-format off
#include <unordered_map>
#include <vector>
#include <string>
#include <filesystem>
#include <memory>
#include "ofMain.h"
#include "config.hpp"
#include "../../../model.hpp"
// clang-format on 


namespace plantmusic {

class player {


  typedef int sound_id;


  std::vector<ofSoundPlayer> sounds {};
  std::unordered_map <chan_id, sound_id> channel_sound {};
  std::unordered_map <std::string, sound_id> sound_names {};
  std::unique_ptr<plantmusic::config> & config;
  public:

  player( std::unique_ptr<plantmusic::config> & config) : config(config) {}

  void play_sound(int id) {
    // todo proper mapping
    // volume control
    id = id % sounds.size();
    if (!sounds[id].isPlaying()) {
      sounds[id].play(); 
    }
  }

  void stop_sound(int id) {
    sounds[id].stop();
  }

  void setup() {
    auto sound_directory = config->sounds_path;

    for (const auto& dir_entry: std::filesystem::directory_iterator(sound_directory)) {
      if (!dir_entry.is_regular_file()) {
        continue;
      }
      ofSoundPlayer &soundPlayer  = sounds.emplace_back();
      bool loaded_ok = soundPlayer.load(dir_entry.path().string());
      if (!loaded_ok) {
        ofLog(OF_LOG_WARNING) << "Failed to load file: " << dir_entry.path().string();
        sounds.pop_back();
        continue;
      }

      sound_names[dir_entry.path().string()] = sounds.size() - 1;
    }

    ofLog() << "Loaded " << sounds.size() << " sounds "; 
    for (auto &s : sound_names) {
        ofLog() << "Loaded  " << s.first; 
    }
          
    // iterate directory
  }
};
};
