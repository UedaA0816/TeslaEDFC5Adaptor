#pragma once

#include "shared/ICanListener.h"

// ============================================================
// CAN ゲートウェイインターフェース
// TWAI 受信・パース・リスナー通知の I/O 責任を抽象化する。
// ============================================================

class ICanGateway {
  public:
    virtual ~ICanGateway() = default;
    virtual bool begin() = 0;
    virtual bool addListener(ICanListener* listener) = 0;
    virtual void update() = 0;
};
