
#pragma once

// clang-format off
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>
#include <filesystem>
#include <ranges>
#include <memory>
#include "ofMain.h"
#include "config.hpp"
#include "../../../model.hpp"
// clang-format on 


namespace plantmusic {

class player {


  typedef int sound_id;


  //std::vector<ofSoundPlayer> sounds {};
  std::unordered_map <chan_id, sound_id> channel_sound {};
  std::unordered_map <std::string, ofSoundPlayer> sounds{};
  std::unique_ptr<plantmusic::config> & config;
  public:

  player( std::unique_ptr<plantmusic::config> & config) : config(config) {}

  void play_sound(chan_id channel) {
    // todo proper mapping
    // volume control


    //channel = channel % sounds.size();
    //if (!sounds[channel].isPlaying()) {
    //  sounds[channel].play();
    //}

    bool played = false;
    for (const auto& [chan, sound] : config->channel_sound_mapping) {
      if (chan != channel) continue;

      if (!sounds.contains(sound)) {
        ofLog(OF_LOG_ERROR) << "Missing sound id for known sound " << sound; 
        continue;
      }

      played = true;
      if (!sounds[sound].isPlaying()) {
        sounds[sound].play(); 
      } else {
        ofLog() << "already playing " << sound;
      }
    }
    if (!played) {
      ofLog(OF_LOG_WARNING) << "No sound mapped to channel " << (int)channel;
    }
  }

  void stop_sound(chan_id channel) {

    for (const auto& [chan, sound] : config->channel_sound_mapping) {
      if (chan != channel) continue;

      if (!sounds.contains(sound)) {
        ofLog(OF_LOG_ERROR) << "Missing sound id for known sound " << sound; 
        continue;
      }

      sounds[sound].stop();
    }

  }

  void setup() {
    static int i = 0;
    auto sound_directory = config->sounds_path;

    for (const auto& dir_entry: std::filesystem::directory_iterator(sound_directory)) {
      if (!dir_entry.is_regular_file()) {
        continue;
      }
      auto sound_name = dir_entry.path().filename().string();
      sounds[sound_name] = ofSoundPlayer();

      ofSoundPlayer &soundPlayer  = sounds[sound_name];
      soundPlayer.load(dir_entry.path().string());
      if (!soundPlayer.isLoaded()) {
        ofLog(OF_LOG_WARNING) << "Failed to load file: " << dir_entry.path().string();
        sounds.erase(sound_name);
        continue;
      }

    }

    ofLog() << "Loaded " << sounds.size() << " sounds " << sounds.size(); 
    for (const auto &[name, s] : sounds) {
        ofLog() << "Loaded  " << name; 
    }
          
    // iterate directory
  }
};
};
