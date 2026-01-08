#pragma once
#include <cstdint>

namespace auraglow {

enum class Mode : uint8_t {
  COLOR = 0,
  MAKEUP = 1,
  KERATIN = 2,
  CUT = 3
};

struct FrameView {
  uint8_t* rgba;     // RGBA8888 in-place buffer (stub path)
  int32_t width;
  int32_t height;
  int32_t stride;    // bytes per row
  float   exposure;
  float   wb_r;
  float   wb_g;
  float   wb_b;
};

struct Params {
  Mode  mode;
  float intensity;     // 0..1
  uint32_t dye_rgba;   // packed RGBA (stub)
};

struct Result {
  int32_t ok;        // 1 = ok, 0 = fail
  int32_t code;      // error code
};

Result process_frame(FrameView frame, Params params) noexcept;

} // namespace auraglow
