#pragma once

// ============================================================
// アプリケーションのライフサイクル IF（board 非依存）
// main.cpp は Arduino の setup()/loop() をこの IF に橋渡しするだけ。
// 具体的な配線（ピン・gateway 生成・DI 登録）は合成ルートに集約する。
// ============================================================

class IApp {
  public:
    virtual ~IApp() = default;
    virtual void setup() = 0;
    virtual void loop() = 0;
};
