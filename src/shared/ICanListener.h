#pragma once

#include "shared/TeslaSignals.h"

// ============================================================
// CAN シグナル通知インターフェース
// feature が CanGateway::addListener() で登録し、コールバックを受ける。
// 各 feature は必要なメソッドだけ override すればよい（デフォルト空実装）。
// ============================================================

class ICanListener {
  public:
    virtual ~ICanListener() = default;
    virtual void onSpeedUpdate(float speedKmh) {}
    virtual void onAccelModeUpdate(AccelModeType mode) {}
};
