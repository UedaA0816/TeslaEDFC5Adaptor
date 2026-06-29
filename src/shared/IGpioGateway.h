#pragma once

#include "shared/ILifeCycle.h"

// ============================================================
// GPIO ゲートウェイインターフェース
// LED・EDFC5 リレー・車速 PWM パルスの出力 I/O を抽象化する。
// begin(): ピン初期化 / loop(): 不要（出力はイベント駆動）。
// ============================================================

class IGpioGateway : public ILifeCycle {
  public:
    virtual ~IGpioGateway() = default;
    virtual void setSpeedPulse(float speedKmh) = 0;
    virtual void setEdfc5Relay(bool ext1, bool ext2) = 0;
    virtual void setLed(bool blue, bool yellow) = 0;
};
