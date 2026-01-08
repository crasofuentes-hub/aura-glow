#include "auraglow.h"
#include <vector>
#include <iostream>

int main() {
  const int w = 160, h = 120;
  std::vector<uint8_t> img(w*h*4, 0);

  // Fill with mid-gray
  for (int i=0;i<w*h;i++){
    img[i*4+0]=120; img[i*4+1]=120; img[i*4+2]=120; img[i*4+3]=255;
  }

  auraglow::FrameView fv{
    img.data(), w, h, w*4, 1.0f, 1.0f, 1.0f, 1.0f
  };
  auraglow::Params p{
    auraglow::Mode::KERATIN, 0.85f, 0xFF00FFFFu // magenta-ish
  };
  auto r = auraglow::process_frame(fv, p);
  std::cout << "OK=" << r.ok << " CODE=" << r.code << "\n";
  std::cout << "PIX0=" << (int)img[0] << "," << (int)img[1] << "," << (int)img[2] << "\n";
  return (r.ok==1 && r.code==0) ? 0 : 1;
}
