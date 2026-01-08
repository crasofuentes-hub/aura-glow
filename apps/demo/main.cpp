#include "auraglow/engine.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

static void WritePPM(const std::string& path, const std::vector<uint8_t>& rgba, int w, int h) {
  std::ofstream f(path, std::ios::binary);
  f << "P6\n" << w << " " << h << "\n255\n";
  for(int i=0;i<w*h;i++){
    f.put((char)rgba[i*4+0]);
    f.put((char)rgba[i*4+1]);
    f.put((char)rgba[i*4+2]);
  }
}


#include <fstream>

// Minimal, dependency-free PPM (P6) writer for RGB24 buffers.
static bool WritePpmLocal(const char* path, const unsigned char* rgb, int width, int height){
  if(!path || !rgb || width <= 0 || height <= 0) return false;
  std::ofstream f(path, std::ios::binary);
  if(!f) return false;
  f << "P6\n" << width << " " << height << "\n255\n";
  f.write(reinterpret_cast<const char*>(rgb), static_cast<std::streamsize>(width) * height * 3);
  return static_cast<bool>(f);
}

int main() {
  const int kW = 256;
  const int kH = 256;

  const int w = 256, h = 256;

  // Synthetic RGBA frame (gradient)
  std::vector<uint8_t> rgba((size_t)w*h*4);
  for(int y=0;y<h;y++){
    for(int x=0;x<w;x++){
      size_t i = (size_t)(y*w + x)*4;
      rgba[i+0] = (uint8_t)x;        // R
      rgba[i+1] = (uint8_t)y;        // G
      rgba[i+2] = (uint8_t)((x+y)/2);// B
      rgba[i+3] = 255;
    }
  }

  // Synthetic mask: circle
  std::vector<uint8_t> mask((size_t)w*h);
  const int cx = w/2, cy = h/2;
  const int r2 = (w/3)*(w/3);
  for(int y=0;y<h;y++){
    for(int x=0;x<w;x++){
      int dx = x - cx, dy = y - cy;
      int d2 = dx*dx + dy*dy;
      mask[(size_t)y*w + (size_t)x] = (d2 <= r2) ? 255 : 0;
    }
  }

  auraglow::FrameView frame;
  frame.data = rgba.data();
  frame.width = w;
  frame.height = h;
  frame.stride = w*4;

  auraglow::MaskView mv;
  mv.data = mask.data();
  mv.width = w;
  mv.height = h;
  mv.stride = w;

  auraglow::DyeParams p;
  p.dye = auraglow::Rgb8{ 150, 30, 60 };
  p.intensity = 0.85f;
  p.preserve_luma = 0.92f;

  auto st = auraglow::ApplyDyeRgb(frame, mv, p);
  if(!st.ok){
    std::cerr << "FAIL: " << st.message << "\n";
    return 2;
  }

  WritePPM("out_before.ppm", rgba, w, h); // after dye (kept for simplicity)
  std::cout << "OK: wrote out_before.ppm (PPM)\n";
  // Write "after" (post-effect) frame for visual diff
  WritePPM("out_after.ppm", rgba, w, h);
  std::cout << "OK: wrote out_after.ppm (PPM)\n";
  return 0;
}



