#pragma once

#include "shared/ICanGateway.h"
#include "shared/ICanListener.h"
#include "shared/IGpioGateway.h"
#include "shared/ILifeCycle.h"
#include "shared/TeslaSignals.h"

// ============================================================
// 加速モード → EDFC5 リレー / ステータス LED 制御 feature
// 必要な gateway はコンストラクタ注入で受け取る。
// ============================================================

class AccelMode : public ILifeCycle, public ICanListener {
  public:
    AccelMode(ICanGateway& can, IGpioGateway& gpio) : _can(can), _gpio(gpio) {}

    void begin() override { _can.addListener(this); }
    void loop() override {}
    void onAccelModeUpdate(AccelModeType mode) override;

  private:
    void applyMode(AccelModeType mode);
    ICanGateway&  _can;
    IGpioGateway& _gpio;
};
