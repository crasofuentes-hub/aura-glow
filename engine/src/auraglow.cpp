#include "auraglow.h"
#include <algorithm>

namespace auraglow {

// Tiny deterministic stub:
// - Does NOT use ML.
// - Applies a visible, reversible effect depending on mode,
//   to prove end-to-end plumbing (FFI later).
static inline void tint_rgba(uint8_t* p, uint32_t rgba, float a) {
  // rgba: 0xRRGGBBAA (we only use RGB)
  uint8_t r = (rgba >> 24) & 0xFF;
  uint8_t g = (rgba >> 16) & 0xFF;
  uint8_t b = (rgba >>  8) & 0xFF;

  // p: RGBA
  p[0] = static_cast<uint8_t>(std::clamp((1.0f-a)*p[0] + a*r, 0.0f, 255.0f));
  p[1] = static_cast<uint8_t>(std::clamp((1.0f-a)*p[1] + a*g, 0.0f, 255.0f));
  p[2] = static_cast<uint8_t>(std::clamp((1.0f-a)*p[2] + a*b, 0.0f, 255.0f));
}

Result process_frame(FrameView frame, Params params) noexcept {
  if (!frame.rgba || frame.width <= 0 || frame.height <= 0 || frame.stride <= 0) {
    return {0, 1001};
  }
  const float a = std::clamp(params.intensity, 0.0f, 1.0f);

  // Mode-specific stub pattern:
  // COLOR: uniform tint over entire frame
  // MAKEUP: tint only top half (simulates face region)
  // KERATIN: add horizontal "specular band" tint
  // CUT: tint only a vertical strip (simulates changed geometry area)
  for (int y = 0; y < frame.height; ++y) {
    uint8_t* row = frame.rgba + y * frame.stride;
    for (int x = 0; x < frame.width; ++x) {
      uint8_t* px = row + x * 4;

      bool apply = false;
      switch (params.mode) {
        case Mode::COLOR:
          apply = true;
          break;
        case Mode::MAKEUP:
          apply = (y < frame.height / 2);
          break;
        case Mode::KERATIN:
          apply = (y > frame.height/3 && y < frame.height/3 + 12);
          break;
        case Mode::CUT:
          apply = (x > frame.width/2 - 20 && x < frame.width/2 + 20);
          break;
        default:
          apply = false;
      }

      if (apply) tint_rgba(px, params.dye_rgba, a);
    }
  }

  return {1, 0};
}

} // namespace auraglow
