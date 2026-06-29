#pragma once

#include <Arduino.h>

#include "shared/IGpioGateway.h"

class GpioGateway : public IGpioGateway {
  public:
    void begin() override;
    void loop() override {}  // 出力はイベント駆動のため周期処理なし
    void setSpeedPulse(float speedKmh) override;
    void setEdfc5Relay(bool ext1, bool ext2) override;
    void setLed(bool blue, bool yellow) override;

  private:
    uint32_t speedToFreq(float kmh);

    // RejsaCAN-ESP32-S3 v3.4 公式ピン配置
    static constexpr gpio_num_t LED_BLUE_PIN    = GPIO_NUM_10;
    static constexpr gpio_num_t LED_YELLOW_PIN  = GPIO_NUM_11;
    // EDFC5 外部入力（暫定ピン・配線確定後に見直し）
    static constexpr gpio_num_t RELAY_EXT1_PIN  = GPIO_NUM_6;   // Sport / ハード方向
    static constexpr gpio_num_t RELAY_EXT2_PIN  = GPIO_NUM_7;   // Chill / ソフト方向
    static constexpr gpio_num_t SPEED_PULSE_PIN = GPIO_NUM_15;  // 車速 PWM 出力

    static constexpr uint8_t  PWM_CHANNEL = 0;
    static constexpr uint32_t PWM_FREQ_HZ = 1000;
    static constexpr uint8_t  PWM_BITS    = 8;
};
