#pragma once

#include "shared/TeslaSignals.h"

// ============================================================
// CAN シグナル通知インターフェース
// feature が CanGateway::addListener() で登録し、コールバックを受ける。
// 各 feature は必要なメソッドだけ override すればよい（デフォルト空実装）。
//
// ⚠️ 並行性コントラクト
// コールバックは CAN 受信専用タスク（コア0）のコンテキストで直接実行される。
// 実装は非ブロッキング・短時間（GPIO 書き込み程度）に留めること。
// 長い処理・待ち・別タスクへの通知が必要な場合はキューを介すること。
// GPIO 書き込みはこの経路（コア0）に一元化されている前提。
// ループ側（コア1）から GPIO を触ると競合するため禁止。
// 将来 _signals 等を別コアから読む場合はミューテックス保護が必要。
// ============================================================

class ICanListener {
  public:
    virtual ~ICanListener() = default;
    virtual void onSpeedUpdate(float speedKmh) {}
    virtual void onAccelModeUpdate(AccelModeType mode) {}
};
