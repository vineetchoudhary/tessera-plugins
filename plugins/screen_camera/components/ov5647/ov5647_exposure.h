#pragma once

#include <cstdint>

namespace esphome::ov5647 {

// Espressif's OV5647 modes can give the line length and pixel clock without a line time.
constexpr float line_time_us(uint32_t tline_ns, uint32_t hts, uint32_t pclk) {
  if (tline_ns != 0) return tline_ns / 1000.0f;
  return pclk == 0 ? 0.0f : hts * 1e6f / pclk;
}

// The OV5647's AEC/AGC registers hold a 20-bit exposure in 1/16 lines and a 10-bit gain in 1/16 steps.
inline bool decode_exposure(const uint8_t (&exposure_regs)[3], const uint8_t (&gain_regs)[2], float line_us,
                            float &exposure_us, float &gain) {
  const uint32_t exposure_x16 = ((exposure_regs[0] & 0x0fu) << 16) | (uint32_t{exposure_regs[1]} << 8) | exposure_regs[2];
  const uint32_t gain_x16 = ((gain_regs[0] & 0x03u) << 8) | gain_regs[1];
  if (line_us <= 0.0f || exposure_x16 == 0 || gain_x16 < 16) return false;
  exposure_us = exposure_x16 * line_us / 16.0f;
  gain = gain_x16 / 16.0f;
  return true;
}

}  // namespace esphome::ov5647
