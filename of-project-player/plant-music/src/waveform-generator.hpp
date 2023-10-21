
#pragma once
#include "json.hpp"
#include "ofLog.h"
#include <fstream>
#include <map>
#include <optional>
#include <span>
#include <stdint.h>
#include <string>
#include <vector>

struct light_sequence {
  std::string filename;
  int numbins;
  std::vector<float> times;
  std::vector<std::vector<uint8_t>> frames;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(light_sequence, filename, numbins, times,
                                 frames);
  std::map<float, std::vector<uint8_t>> keyframes;

  static std::optional<light_sequence> load(std::string filename) {
    std::ifstream fs{filename};

    light_sequence s;
    try {
      nlohmann::json j;
      fs >> j;
      s = j;
    } catch (std::runtime_error &e) {
      ofLog(OF_LOG_ERROR) << "Malformed light pattern " << e.what();
      return {};
    }

    if (s.frames.size() != s.times.size()) {
      ofLog(OF_LOG_ERROR)
          << "Malformed light sequence: times and frames size do not match";
      return {};
    }

    for (int i = 0; i < s.frames.size(); i++) {
      s.keyframes[s.times[i]] = s.frames[i];
    }

    return s;
  }

  const std::reference_wrapper<std::vector<uint8_t>>
  get_keyframe_for_time(float timepoint_seconds) {
    return keyframes.lower_bound((int)(1000 * timepoint_seconds))->second;
  }

  void write_keyframe_for_timepoint(float timepoint_seconds,
                                    std::vector<uint8_t> &out) {
    const std::reference_wrapper<std::vector<uint8_t>> kf =
        get_keyframe_for_time(timepoint_seconds);

    int end = kf.get().size() > out.size() ? out.size() : kf.get().size();

    for (int i = 0; i < end; i++) {
      out[i] = kf.get()[i];
    }

    if (out.size() > kf.get().size()) {
      // clamp
      for (int i = kf.get().size(); i < out.size(); i++) {
        out[i] = kf.get()[kf.get().size() - 1];
      }
    }
    // else truncate
  }
};
