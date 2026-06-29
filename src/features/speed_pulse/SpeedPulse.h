#pragma once

#include "shared/ICanGateway.h"
#include "shared/ICanListener.h"
#include "shared/IGpioGateway.h"
#include "shared/ILifeCycle.h"

// ============================================================
// 車速 → EDFC5 車速パルス変換 feature
// 必要な gateway はコンストラクタ注入で受け取る。
// ============================================================

class SpeedPulse : public ILifeCycle, public ICanListener {
  public:
    SpeedPulse(ICanGateway& can, IGpioGateway& gpio) : _can(can), _gpio(gpio) {}

    void begin() override { _can.addListener(this); }
    void loop() override {}
    void onSpeedUpdate(float speedKmh) override { _gpio.setSpeedPulse(speedKmh); }

  private:
    ICanGateway&  _can;
    IGpioGateway& _gpio;
};
