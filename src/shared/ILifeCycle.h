#pragma once

// ============================================================
// コンポーネント共通のライフサイクル IF
// feature・gateway 双方が実装し、App が順序付きリストとして
// begin() 一括 → loop() 一括で駆動する。
// ============================================================

class ILifeCycle {
  public:
    virtual ~ILifeCycle() = default;
    virtual void begin() = 0;
    virtual void loop() = 0;
};
