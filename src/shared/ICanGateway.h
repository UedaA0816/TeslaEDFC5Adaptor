#pragma once

#include "shared/ICanListener.h"
#include "shared/ILifeCycle.h"

// ============================================================
// CAN ゲートウェイインターフェース
// TWAI 受信・パース・リスナー通知の I/O 責任を抽象化する。
// begin(): ドライバ初期化 + 受信専用タスク起動 / loop(): 未使用（受信はタスクが担う）。
// ============================================================

class ICanGateway : public ILifeCycle {
  public:
    virtual ~ICanGateway() = default;
    virtual bool addListener(ICanListener* listener) = 0;
};
