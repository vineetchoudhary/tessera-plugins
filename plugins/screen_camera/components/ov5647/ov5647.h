#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"
#include "ov5647_window.h"

namespace esphome::ov5647 {

// Keeps the OV5647's colour order through format and flip changes (ov5647_window.h). Once it has found the sensor,
// it also reads the exposure and gain for sensing. Both run on the capture task through camera_sensor.h.
class OV5647 : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  void controls_set();
  bool read_exposure(float line_us, float &exposure_us, float &gain);
  // After esp_video_camera (DATA), whose setup starts the sensor.
  float get_setup_priority() const override { return setup_priority::PROCESSOR; }

 protected:
  bool read_(uint16_t reg, uint8_t &value);
  bool write_(uint16_t reg, uint8_t value);

  bool found_{false};
  bool wrote_{false};     // a window was written since boot
  Window own_{0, 0};      // the format's own window
  Window written_{0, 0};  // the window written last
  int flips_{-1};         // the flips last seen, for the log: bit 0 upside down, bit 1 mirrored
  bool failed_{false};    // the sensor did not answer, said once
};

}  // namespace esphome::ov5647
