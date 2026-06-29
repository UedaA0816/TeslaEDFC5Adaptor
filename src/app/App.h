#pragma once

#include <stddef.h>

#include "shared/IApp.h"
#include "shared/ILifeCycle.h"

// ============================================================
// 汎用ライフサイクルランナー（board 非依存）
// 合成ルートが構築した ILifeCycle* の順序付きリストを受け取り、
// begin() 一括 → loop() 一括で駆動するだけ。
// 個別の feature/gateway 型・DIContainer には依存しない。
// ============================================================

class App : public IApp {
  public:
    App(ILifeCycle* const* components, size_t count)
        : _components(components), _count(count) {}

    void setup() override;
    void loop() override;

  private:
    ILifeCycle* const* _components;  // 非所有（実体は合成ルートが保持）
    size_t _count;
};
