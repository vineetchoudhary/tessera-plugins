// clang++ -std=c++17 -Wall -Wextra -Werror -I components tests/test_ov5647_exposure.cpp -o /tmp/test_ov5647_exposure
#include "ov5647/ov5647_exposure.h"

#include <cassert>
#include <cmath>
#include <cstdio>

using esphome::ov5647::decode_exposure;
using esphome::ov5647::line_time_us;

int main() {
  // The active format's line time wins. The stock 800x800 mode reports a line length and clock instead.
  assert(line_time_us(12500, 984, 81666700) == 12.5f);
  assert(std::fabs(line_time_us(0, 984, 81666700) - 12.049f) < 0.001f);
  assert(line_time_us(0, 984, 0) == 0.0f);

  // 400 lines at 12.5 microseconds per line and unity gain.
  float exposure_us = 0, gain = 0;
  assert(decode_exposure({0x00, 0x19, 0x00}, {0x00, 0x10}, 12.5f, exposure_us, gain));
  assert(exposure_us == 5000.0f);
  assert(gain == 1.0f);

  // Reserved bits are ignored, including when the reading uses the high bits and a fractional line.
  assert(decode_exposure({0xf1, 0x00, 0x08}, {0xfd, 0x18}, 12.5f, exposure_us, gain));
  assert(exposure_us == 51206.25f);
  assert(gain == 17.5f);

  assert(!decode_exposure({0x00, 0x19, 0x00}, {0x00, 0x10}, 0.0f, exposure_us, gain));
  assert(!decode_exposure({0x00, 0x00, 0x00}, {0x00, 0x10}, 12.5f, exposure_us, gain));
  assert(!decode_exposure({0x00, 0x19, 0x00}, {0x00, 0x00}, 12.5f, exposure_us, gain));
  std::puts("ov5647_exposure: ok");
}
