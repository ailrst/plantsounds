
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

namespace plantsounds {
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

  const std::vector<uint8_t> &
  get_keyframe_for_time_millis(int timepoint_millis) const {
    auto res = keyframes.upper_bound((int)(1000 * timepoint_millis));
    if (res == keyframes.end()) {
      return keyframes.begin()->second;
    } else {
      return res->second;
    }
  }

  void write_keyframe_for_timepoint_millis(int timepoint_millis,
                                           std::vector<uint8_t> &out) {
    const std::vector<uint8_t> &kf =
        get_keyframe_for_time_millis(timepoint_millis);

    int end = kf.size() > out.size() ? out.size() : kf.size();

    for (int i = 0; i < end; i++) {
      out[i] = kf[i];
    }

    if (out.size() > kf.size()) {
      // clamp
      for (int i = kf.size(); i < out.size(); i++) {
        out[i] = kf[kf.size() - 1];
      }
    }
    // else truncate
  }
};
}; // namespace plantsounds
