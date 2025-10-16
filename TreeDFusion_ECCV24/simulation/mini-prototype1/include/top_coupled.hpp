#pragma once
#include <memory>
#include <typeindex>
#include <vector>
#include <ostream>

#include <cadmium/modeling/coupling.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_coupled.hpp>
#include <cadmium/modeling/dynamic_atomic.hpp>

#include "growth_atomic.hpp"
#include "messages.hpp"

// Top-level out port (runner doesn't use it, but dynamic_coupled wants port lists)
struct top_defs { struct o_frame : public cadmium::out_port<FrameOut> {}; };

/*** One-shot source that emits SimConfig at t=0 ***/
template<typename TIME>
class ConfigSource {
public:
  struct defs { struct out : public cadmium::out_port<SimConfig> {}; };
  using input_ports  = std::tuple<>;
  using output_ports = std::tuple<typename defs::out>;

  struct state_type { bool sent=false; SimConfig cfg{}; } state;

  // <<< NEW: allow Cadmium to print the state for debugging >>>
  friend std::ostream& operator<<(std::ostream& os, const state_type& s) {
    os << "{sent:" << (s.sent ? "true" : "false")
       << ", jobId:" << s.cfg.jobId
       << ", shape:" << s.cfg.shape
       << ", baseSize:" << s.cfg.baseSize
       << ", rate:" << s.cfg.rate
       << ", tStart:" << s.cfg.tStart
       << ", tEnd:" << s.cfg.tEnd
       << ", fps:" << s.cfg.fps
       << "}";
    return os;
  }
  // >>> END NEW

  ConfigSource() = default;
  explicit ConfigSource(const SimConfig& c) { state.cfg = c; }

  TIME time_advance() const { return state.sent ? TIME::infinity() : TIME(); }
  void internal_transition() { state.sent = true; }
  void external_transition(TIME, typename cadmium::make_message_bags<input_ports>::type) {}
  void confluence_transition(TIME, typename cadmium::make_message_bags<input_ports>::type) {
    internal_transition();
  }

  typename cadmium::make_message_bags<output_ports>::type output() const {
    typename cadmium::make_message_bags<output_ports>::type bags;
    if (!state.sent) cadmium::get_messages<typename defs::out>(bags).push_back(state.cfg);
    return bags;
  }
};

/*** Build the top coupled model ***/
template<typename TIME>
std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> build_top() {
  using namespace cadmium::dynamic::modeling;
  using cadmium::dynamic::translate::make_dynamic_atomic_model; // factory uses <Atomic, TIME>
  using cadmium::dynamic::translate::make_IC;
  using cadmium::dynamic::translate::make_EOC;

  // Scenario (edit values as needed)
  SimConfig cfg{
    "job-001", "cube",
    /*baseSize*/ 1.0,
    /*rate*/     0.2,
    /*tStart*/   0.0,
    /*tEnd*/     5.0,
    /*fps*/      30.0
  };

  auto cfgSrc = make_dynamic_atomic_model<ConfigSource, TIME>("cfgSrc", cfg);
  auto growth = make_dynamic_atomic_model<GrowthAtomic, TIME>("growth");

  // submodels
  Models submodels = {cfgSrc, growth};

  // TOP has no inputs; has one output port type
  Ports in_ports  = {};
  Ports out_ports = { std::type_index(typeid(top_defs::o_frame)) };

  // Couplings
  EICs eics = {};
  EOCs eocs = { make_EOC<growth_defs::o_frame, top_defs::o_frame>("growth") };
  ICs  ics  = { make_IC<typename ConfigSource<TIME>::defs::out, growth_defs::i_config>("cfgSrc","growth") };

  // id, submodels, in_ports, out_ports, eics, eocs, ics
  return std::make_shared<coupled<TIME>>("TOP", submodels, in_ports, out_ports, eics, eocs, ics);
}
