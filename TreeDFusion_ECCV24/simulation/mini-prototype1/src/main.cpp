#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_coupled.hpp>

#include "top_coupled.hpp"
#include "messages.hpp"
#include <NDTime.hpp>

using TIME = NDTime;

int main() {
  auto top = build_top<TIME>();
  cadmium::dynamic::engine::runner<TIME> r(top, TIME{});
  r.run_until(TIME::infinity());
  return 0;
}
