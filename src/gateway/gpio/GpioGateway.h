#pragma once

#include <Arduino.h>

#include "shared/IGateway.h"

class GpioGateway : public IGpioGateway {
  public:
    void begin();
    void setSpeedPulse(float speedKmh) override;
    void setEdfc5Relay(bool ext1, bool ext2) override;
    void setLed(bool blue, bool yellow) override;

  private:
    uint32_t speedToFreq(float kmh);

    static constexpr gpio_num_t LED_BLUE_PIN   = GPIO_NUM_48;
    static constexpr gpio_num_t LED_YELLOW_PIN = GPIO_NUM_47;
    static constexpr gpio_num_t RELAY_EXT1_PIN = GPIO_NUM_21;
    static constexpr gpio_num_t RELAY_EXT2_PIN = GPIO_NUM_20;
    static constexpr gpio_num_t SPEED_PULSE_PIN = GPIO_NUM_1;

    static constexpr uint8_t PWM_CHANNEL = 0;
    static constexpr uint32_t PWM_FREQ_HZ = 1000;
    static constexpr uint8_t PWM_BITS = 8;
};
