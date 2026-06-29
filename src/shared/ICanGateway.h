#pragma once

#include "shared/ICanListener.h"
#include "shared/ILifeCycle.h"

// ============================================================
// CAN ゲートウェイインターフェース
// TWAI 受信・パース・リスナー通知の I/O 責任を抽象化する。
// begin(): ドライバ初期化 / loop(): 受信ポーリング＋通知。
// ============================================================

class ICanGateway : public ILifeCycle {
  public:
    virtual ~ICanGateway() = default;
    virtual bool addListener(ICanListener* listener) = 0;
};
