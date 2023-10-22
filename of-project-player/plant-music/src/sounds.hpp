
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
#include <optional>
#include "waveform-generator.hpp"
#include "../../../model.hpp"
// clang-format on 


namespace plantmusic {

struct sound {
  ofSoundPlayer player;
  std::string filename;
  chan_id channel;
  std::optional<plantsounds::light_sequence> lights {};
};

class player {


  typedef int sound_id;


  std::unique_ptr<plantmusic::config> & config;
  public:
  std::unordered_map <int, sound> sounds_mapping{};
  std::vector<std::string> sound_names {};

  player( std::unique_ptr<plantmusic::config> & config) : config(config) {}

  void play_sound(chan_id channel) {
    // volume control
    
    if (!sounds_mapping.contains(channel)) {
      ofLog(OF_LOG_WARNING) << "No sound mapped to channel " << (int)channel;
      return;
    }

    if (!sounds_mapping[channel].player.isPlaying()) {
      sounds_mapping[channel].player.play();
    }

  }

  void stop_sound(chan_id channel) {
    if (!sounds_mapping.contains(channel)) {
      ofLog(OF_LOG_WARNING) << "No sound mapped to channel " << (int)channel;
      return;
    }
    sounds_mapping[channel].player.stop(); 
  }

  void reload_config() {
    for (const auto& [chan, sound] : config->channel_sound_mapping) {
  //    std::unordered_map <int, ofSoundPlayer> sounds_mapping{};
      
      chan_id channel = chan;
      sounds_mapping[chan] = {
        .player=ofSoundPlayer(), 
        .filename=sound,
        .channel=channel,
      };
      sounds_mapping[chan].player.load((std::filesystem::path(config->sounds_path) / sound).string());

      std::stringstream pattern_path;
      pattern_path << sound.substr(0, sound.rfind(".")) << ".json";

    auto full_pattern_path = (std::filesystem::path(config->sounds_path) / pattern_path.str());
      if (std::filesystem::exists(full_pattern_path)) {
        sounds_mapping[chan].lights = plantsounds::light_sequence::load(full_pattern_path.string());
        ofLog() << "Loaded light pattern for " << sound << " (" << pattern_path.str() << ") " 
          << sounds_mapping[chan].lights->frames.size() << "x" << sounds_mapping[chan].lights->get_keyframe_for_time_millis(0).size();
      } else {
        ofLog() << "No light pattern for " << sound<< " (" << pattern_path.str() << ")";
      }



      if (!sounds_mapping[chan].player.isLoaded()) {
        ofLog(OF_LOG_WARNING) << "Failed to load file: " << sound;
        sounds_mapping.erase(chan);
      }
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
      sound_names.push_back(sound_name);
    }

    reload_config();
    ofLog() << "Loaded " << sounds_mapping.size(); 
    for (const auto &player: sounds_mapping) {
        ofLog() << "Loaded  " << player.second.filename << " on chan " << (int)player.second.channel ; 
    }
          
    // iterate directory
  }
};
};
