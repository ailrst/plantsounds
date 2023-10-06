
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


  std::vector<ofSoundPlayer> sounds {};
  std::unordered_map <chan_id, sound_id> channel_sound {};
  std::unordered_map <std::string, sound_id> sound_names {};
  std::unique_ptr<plantmusic::config> & config;
  public:

  player( std::unique_ptr<plantmusic::config> & config) : config(config) {}

  void play_sound(chan_id channel) {
    // todo proper mapping
    // volume control

    channel = channel % sounds.size();
    sounds[channel].play();
    return;
    ofLog() << "Play sound";
    bool played = false;
    for (const auto& [chan, sound] : config->channel_sound_mapping) {
      if (chan == channel) {

        if (!sound_names.contains(sound)) {
          ofLog(OF_LOG_ERROR) << "Missing sound id for known sound " << sound; 
          continue;
        }

        auto sound_id = sound_names[sound];
        sounds[sound_id].play(); 
        if (!sounds[sound_id].isPlaying()) {
          played = true;
        } else {
          ofLog() << "already playing " << sound << " id " << sound_id;
        }
      }
    }
    if (!played) {
      ofLog(OF_LOG_WARNING) << "No sound mapped to channel " << (int)channel;
    }
  }

  void stop_sound(chan_id channel) {
    channel = channel % sounds.size();
    sounds[channel].stop();
  }

  void setup() {
    static int i = 0;
    auto sound_directory = config->sounds_path;

    for (const auto& dir_entry: std::filesystem::directory_iterator(sound_directory)) {
      if (!dir_entry.is_regular_file()) {
        continue;
      }
      ofSoundPlayer &soundPlayer  = sounds.emplace_back();
      soundPlayer.play();
      bool loaded_ok = soundPlayer.load(dir_entry.path().string());
      if (!loaded_ok) {
        ofLog(OF_LOG_WARNING) << "Failed to load file: " << dir_entry.path().string();
        sounds.pop_back();
        continue;
      }

      auto sound_name = dir_entry.path().filename().string();
      sound_names[sound_name] = sounds.size() - 1;

      if (std::none_of(config->channel_sound_mapping.begin(),
      config->channel_sound_mapping.end(), 
      [sound_name](auto &e) -> bool {return e.second == sound_name;})) {
        config->channel_sound_mapping.push_back({i++, sound_name}); 
      }
    }

    ofLog() << "Loaded " << sounds.size() << " sounds " << sound_names.size(); 
    for (auto &s : sound_names) {
        ofLog() << "Loaded  " << s.first; 
    }
          
    // iterate directory
  }
};
};
