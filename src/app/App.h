#pragma once

#include <vector>

#include "shared/IApp.h"
#include "shared/ILifeCycle.h"

// ============================================================
// 汎用ライフサイクルランナー（board 非依存）
// 合成ルートが構築した ILifeCycle コンポーネント一覧を受け取り、
// begin() 一括 → loop() 一括で駆動するだけ。
// 個別の feature/gateway 型・DIContainer には依存しない。
// ============================================================

class App : public IApp {
  public:
    explicit App(const std::vector<ILifeCycle*>& components)
        : _components(components) {}

    void setup() override;
    void loop() override;

  private:
    std::vector<ILifeCycle*> _components;  // ポインタのみコピー保持（実体は DI コンテナが所有）
};
