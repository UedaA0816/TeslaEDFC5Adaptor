#pragma once

#include <Arduino.h>
#include <driver/twai.h>

#include "shared/ICanGateway.h"
#include "shared/TeslaSignals.h"

class CanGateway : public ICanGateway {
  public:
    CanGateway(gpio_num_t txPin, gpio_num_t rxPin, gpio_num_t rsPin);

    void begin() override;
    bool addListener(ICanListener* listener) override;
    void loop() override;

  private:
    float parseVehicleSpeed(const twai_message_t& msg);
    AccelModeType parseAccelMode(const twai_message_t& msg);
    void notifySpeed(float kmh);
    void notifyMode(AccelModeType mode);
    void updateRxRate();

    gpio_num_t _txPin;
    gpio_num_t _rxPin;
    gpio_num_t _rsPin;

    static constexpr uint8_t MAX_LISTENERS = 8;
    ICanListener* _listeners[MAX_LISTENERS]{};
    uint8_t _listenerCount = 0;
    TeslaSignals _signals{};
    uint32_t _rxCount = 0;
    uint32_t _lastRateMs = 0;

    // 車速: DI_vehicleSpeed @ 0x257（Tesla 公式 CD-20-17-001 / 実走実証済み）
    static constexpr uint32_t CAN_SPEED_ID = 0x257;
    // 加速モード: Phase 1 の全ダンプで ID を特定後に実測値へ更新する
    static constexpr uint32_t CAN_MODE_ID = 0x000;
};
