#pragma once

#include <Arduino.h>
#include <driver/twai.h>

#include "shared/ICanGateway.h"

class CanGateway : public ICanGateway {
  public:
    CanGateway(gpio_num_t txPin, gpio_num_t rxPin, gpio_num_t rsPin);

    void start() override;
    bool begin() override;
    bool addListener(ICanListener* listener) override;
    void update() override;

  private:
    float parseVehicleSpeed(const twai_message_t& msg);
    AccelModeType parseAccelMode(const twai_message_t& msg);
    void notifySpeed(float kmh);
    void notifyMode(AccelModeType mode);
    void updateRxRate();

    gpio_num_t _txPin;
    gpio_num_t _rxPin;
    gpio_num_t _rsPin;

    ICanListener* _listeners[8]{};
    uint8_t _listenerCount = 0;
    TeslaSignals _signals{};
    uint32_t _rxCount = 0;
    uint32_t _lastRateMs = 0;

    static constexpr uint8_t MAX_LISTENERS = 8;
    static constexpr uint32_t CAN_SPEED_ID = 0x18FF50E5;
    static constexpr uint32_t CAN_MODE_ID = 0x000;
};
