#include "auraglow/engine.hpp"
#include <algorithm>
#include <cmath>

#if defined(AURAGLOW_ENABLE_RUST)
  #include "auraglow/engine_rust.h"
#endif

namespace auraglow {

static inline float clamp01(float v) { return std::max(0.0f, std::min(1.0f, v)); }
static inline uint8_t clampU8(int v) { return (uint8_t)std::max(0, std::min(255, v)); }

static inline float luminance(float r, float g, float b) {
  return 0.299f*r + 0.587f*g + 0.114f*b; // Rec.601
}

static Status ApplyDyeRgbCpp(FrameView frame, const MaskView& mask, const DyeParams& p) {
  if (!frame.data) return {false, "FrameView.data is null"};
  if (frame.width <= 0 || frame.height <= 0) return {false, "Invalid frame dimensions"};
  if (frame.stride <= 0) return {false, "Invalid frame stride"};
  if (!mask.data) return {false, "MaskView.data is null"};
  if (mask.width != frame.width || mask.height != frame.height) return {false, "Mask dimensions must match frame"};
  if (mask.stride <= 0) return {false, "Invalid mask stride"};

  const float intensity = clamp01(p.intensity);
  const float preserve  = clamp01(p.preserve_luma);

  const float dr = p.dye.r / 255.0f;
  const float dg = p.dye.g / 255.0f;
  const float db = p.dye.b / 255.0f;

  // Assumption: RGBA8 (4 bytes/pixel)
  for (int y = 0; y < frame.height; ++y) {
    uint8_t* row = frame.data + y * frame.stride;
    const uint8_t* mrow = mask.data + y * mask.stride;

    for (int x = 0; x < frame.width; ++x) {
      const int mi = mrow[x]; // 0..255
      if (mi == 0) { row += 4; continue; }

      const float wMask = (mi / 255.0f);
      const float w = intensity * wMask;

      float r = row[0] / 255.0f;
      float g = row[1] / 255.0f;
      float b = row[2] / 255.0f;

      const float baseLum = luminance(r,g,b);

      // Fast dye model: multiplicative + soft lift (stable in real-time)
      float dyedR = r * (0.5f + 0.5f*dr);
      float dyedG = g * (0.5f + 0.5f*dg);
      float dyedB = b * (0.5f + 0.5f*db);

      // Preserve luminance to avoid "helmet color"
      const float dyedLum = std::max(1e-6f, luminance(dyedR,dyedG,dyedB));
      const float lumScale = baseLum / dyedLum;

      float adjR = std::min(1.0f, dyedR * lumScale);
      float adjG = std::min(1.0f, dyedG * lumScale);
      float adjB = std::min(1.0f, dyedB * lumScale);

      // Blend raw dyed vs lum-preserved dyed
      float outR = dyedR*(1.0f-preserve) + adjR*preserve;
      float outG = dyedG*(1.0f-preserve) + adjG*preserve;
      float outB = dyedB*(1.0f-preserve) + adjB*preserve;

      // Final mix back to original
      r = r*(1.0f-w) + outR*w;
      g = g*(1.0f-w) + outG*w;
      b = b*(1.0f-w) + outB*w;

      row[0] = clampU8((int)std::lround(r * 255.0f));
      row[1] = clampU8((int)std::lround(g * 255.0f));
      row[2] = clampU8((int)std::lround(b * 255.0f));
      // alpha unchanged
      row += 4;
    }
  }

  return {true, "OK (cpp)"};
}

#if defined(AURAGLOW_ENABLE_RUST)
static const char* RustCodeToMsg(int32_t code) {
  switch(code) {
    case 0:  return "OK (rust)";
    case -1: return "Rust: null pointer input";
    case -2: return "Rust: invalid dimensions";
    case -3: return "Rust: mask/frame size mismatch";
    case -4: return "Rust: invalid stride";
    case -5: return "Rust: overflow guard";
    case -6: return "Rust: frame stride < width*4";
    case -7: return "Rust: mask stride < width";
    default: return "Rust: unknown error";
  }
}
#endif

Status ApplyDyeRgb(FrameView frame, const MaskView& mask, const DyeParams& p) {
#if defined(AURAGLOW_ENABLE_RUST)
  // Llamada Rust primero (FFI)
  auraglow_frame_view fv;
  fv.data   = frame.data;
  fv.width  = frame.width;
  fv.height = frame.height;
  fv.stride = frame.stride;

  auraglow_mask_view mv;
  mv.data   = mask.data;
  mv.width  = mask.width;
  mv.height = mask.height;
  mv.stride = mask.stride;

  auraglow_dye_params dp;
  dp.dye.r = p.dye.r;
  dp.dye.g = p.dye.g;
  dp.dye.b = p.dye.b;
  dp.intensity     = p.intensity;
  dp.preserve_luma = p.preserve_luma;

  auraglow_status st = auraglow_apply_dye_rgba(fv, mv, dp);
  if (st.ok) {
    return {true, "OK (rust)"};
  }

  // Si Rust falla, cae al algoritmo C++ (para no romper el demo)
  Status fb = ApplyDyeRgbCpp(frame, mask, p);
  if(!fb.ok) {
    return {false, std::string(RustCodeToMsg(st.code)) + " | fallback failed: " + fb.message};
  }
  return {true, std::string(RustCodeToMsg(st.code)) + " | fallback OK (cpp)"};
#else
  return ApplyDyeRgbCpp(frame, mask, p);
#endif
}

} // namespace auraglow
