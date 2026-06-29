#pragma once

#include "shared/GatewaySet.h"
#include "shared/ICanListener.h"
#include "shared/ILifeCycle.h"
#include "shared/TeslaSignals.h"

// ============================================================
// 加速モード → EDFC5 リレー / ステータス LED 制御 feature
// GatewaySet から使う gateway だけメンバに引き出す。
// ============================================================

class AccelMode : public ILifeCycle, public ICanListener {
  public:
    explicit AccelMode(const GatewaySet& gw) : _can(gw.can), _gpio(gw.gpio) {}

    void begin() override { _can.addListener(this); }
    void loop() override {}
    void onAccelModeUpdate(AccelModeType mode) override;

  private:
    void applyMode(AccelModeType mode);
    ICanGateway&  _can;
    IGpioGateway& _gpio;
};
