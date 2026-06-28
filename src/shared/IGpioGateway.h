#pragma once

#include "shared/TeslaSignals.h"

// ============================================================
// GPIO ゲートウェイインターフェース
// LED・EDFC5 リレー・車速 PWM パルスの出力 I/O を抽象化する。
// ============================================================

class IGpioGateway {
  public:
    virtual ~IGpioGateway() = default;
    virtual void begin() = 0;
    virtual void setSpeedPulse(float speedKmh) = 0;
    virtual void setEdfc5Relay(bool ext1, bool ext2) = 0;
    virtual void setLed(bool blue, bool yellow) = 0;
};
