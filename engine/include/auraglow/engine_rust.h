#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct auraglow_status {
  bool ok;
  int32_t code;
} auraglow_status;

typedef struct auraglow_rgb8 { uint8_t r,g,b; } auraglow_rgb8;

typedef struct auraglow_dye_params {
  auraglow_rgb8 dye;
  float intensity;
  float preserve_luma;
} auraglow_dye_params;

typedef struct auraglow_frame_view {
  uint8_t* data;
  int32_t width;
  int32_t height;
  int32_t stride;
} auraglow_frame_view;

typedef struct auraglow_mask_view {
  const uint8_t* data;
  int32_t width;
  int32_t height;
  int32_t stride;
} auraglow_mask_view;

auraglow_status auraglow_apply_dye_rgba(
  auraglow_frame_view frame,
  auraglow_mask_view mask,
  auraglow_dye_params p
);

#ifdef __cplusplus
}
#endif
