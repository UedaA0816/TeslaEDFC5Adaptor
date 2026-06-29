#pragma once

#include "shared/GatewaySet.h"
#include "shared/ICanListener.h"
#include "shared/ILifeCycle.h"

// ============================================================
// 車速 → EDFC5 車速パルス変換 feature
// GatewaySet から使う gateway だけメンバに引き出す。
// ============================================================

class SpeedPulse : public ILifeCycle, public ICanListener {
  public:
    explicit SpeedPulse(const GatewaySet& gw) : _can(gw.can), _gpio(gw.gpio) {}

    void begin() override { _can.addListener(this); }
    void loop() override {}
    void onSpeedUpdate(float speedKmh) override { _gpio.setSpeedPulse(speedKmh); }

  private:
    ICanGateway&  _can;
    IGpioGateway& _gpio;
};
