#pragma once
#include <string>
#include <cassert>

struct SimConfig {
  std::string jobId;
  std::string shape;
  double baseSize;
  double rate;
  double tStart;
  double tEnd;
  double fps;

  SimConfig() = default;
  SimConfig(std::string jobId_, std::string shape_, double baseSize_, double rate_,
            double tStart_, double tEnd_, double fps_)
      : jobId(std::move(jobId_)), shape(std::move(shape_)), baseSize(baseSize_),
        rate(rate_), tStart(tStart_), tEnd(tEnd_), fps(fps_) {
    assert(fps_ > 0.0);
    assert(tEnd_ >= tStart_);
    assert(baseSize_ > 0.0);
  }
};

// Rectangular prism frame: constant width/depth, growing height
struct FrameOut {
  std::string jobId;
  double t;
  double width;   // constant = baseSize
  double depth;   // constant = baseSize
  double height;  // grows over time
  std::string shape; // "rect_prism"
};
