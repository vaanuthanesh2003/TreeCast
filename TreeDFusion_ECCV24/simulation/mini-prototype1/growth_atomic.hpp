#pragma once
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <limits>
#include <string>
#include <tuple>

#include "messages.hpp"

struct growth_defs {
  struct i_config : public cadmium::in_port<SimConfig> {};
  struct o_frame  : public cadmium::out_port<FrameOut> {};
};

template<typename TIME>
class GrowthAtomic {
public:
  using input_ports  = std::tuple<growth_defs::i_config>;
  using output_ports = std::tuple<growth_defs::o_frame>;

  // Convert seconds (double) to NDTime-style "HH:MM:SS:ms"
  static TIME sec_to_time(double sec) {
    long long total_ms = static_cast<long long>(std::llround(sec * 1000.0));
    if (total_ms < 0) total_ms = 0;
    int hh = static_cast<int>(total_ms / 3600000); total_ms %= 3600000;
    int mm = static_cast<int>(total_ms / 60000);   total_ms %= 60000;
    int ss = static_cast<int>(total_ms / 1000);    int ms = static_cast<int>(total_ms % 1000);
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d:%03d", hh, mm, ss, ms);
    return TIME(std::string(buf));
  }

  struct state_type {
    bool active = false;
    double now = 0.0;
    double t_end = 0.0;
    double dt = 0.0;     // 1/fps
    double base_size = 1.0; // width & depth use this
    double rate = 0.0;      // growth rate (units/sec)
    double scale = 1.0;     // height scale multiplier
    std::string shape = "rect_prism";
    std::string jobId;
  } state;

  // allow Cadmium to print the state for debugging
  friend std::ostream& operator<<(std::ostream& os, const state_type& s) {
    os << "{active:" << (s.active ? "true" : "false")
       << ", now:" << s.now
       << ", t_end:" << s.t_end
       << ", dt:" << s.dt
       << ", base_size:" << s.base_size
       << ", rate:" << s.rate
       << ", scale:" << s.scale
       << ", shape:" << s.shape
       << ", jobId:" << s.jobId
       << "}";
    return os;
  }

  GrowthAtomic() = default;

  TIME time_advance() const {
    return state.active ? sec_to_time(state.dt) : TIME::infinity();
  }

  void internal_transition() {
    state.now += state.dt;
    const double denom = std::max(1e-9, state.base_size);
    // scale(t) so that height = base_size * scale
    state.scale = 1.0 + (state.rate / denom) * (state.now);
    if (state.now + 1e-12 >= state.t_end) state.active = false;
  }

  void external_transition(
      TIME,
      typename cadmium::make_message_bags<input_ports>::type mbs) {
    auto& cfgs = cadmium::get_messages<growth_defs::i_config>(mbs);
    for (const auto& cfg : cfgs) {
      state.jobId     = cfg.jobId;
      // whatever the incoming shape, we output a rectangular prism
      state.shape     = "rect_prism";
      state.base_size = cfg.baseSize;
      state.rate      = cfg.rate;
      state.now       = cfg.tStart;
      state.t_end     = cfg.tEnd;
      state.dt        = 1.0 / cfg.fps;
      state.scale     = 1.0;
      state.active    = (state.t_end > state.now);
    }
  }

  // Cadmium expects (TIME e, bags)
  void confluence_transition(
      TIME,
      typename cadmium::make_message_bags<input_ports>::type mbs) {
    internal_transition();
    external_transition(TIME{}, std::move(mbs));
  }

  typename cadmium::make_message_bags<output_ports>::type output() const {
    typename cadmium::make_message_bags<output_ports>::type bags;
    if (state.active) {
      const double width  = state.base_size;
      const double depth  = state.base_size;
      const double height = state.base_size * state.scale;

      FrameOut f{
        state.jobId,
        state.now + state.dt,
        width,
        depth,
        height,
        state.shape
      };

      // Emit NDJSON line (width/depth constant; height grows)
      std::cout << "{"
                << "\"jobId\":\""  << f.jobId  << "\","
                << "\"t\":"         << f.t      << ","
                << "\"width\":"     << f.width  << ","
                << "\"depth\":"     << f.depth  << ","
                << "\"height\":"    << f.height << ","
                << "\"shape\":\""   << f.shape  << "\""
                << "}\n";

      cadmium::get_messages<growth_defs::o_frame>(bags).push_back(f);
    }
    return bags;
  }
};
