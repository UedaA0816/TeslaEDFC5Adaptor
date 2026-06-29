// ============================================================
// app/App.cpp
// ============================================================

#include <Arduino.h>

#include "app/App.h"

void App::setup() {
    Serial.begin(115200);
    delay(1500);
    Serial.println("\n=== RejsaCAN Tesla CAN × EDFC5 ===");

    // 登録順に begin（gateway → feature の順に初期化される）
    for (auto* c : _components) c->begin();

    Serial.println("=== 初期化完了 ===");
    Serial.println("  時刻(ms)  |  CAN ID  | データ");
    Serial.println("------------|----------|-------------------");
}

void App::loop() {
    for (auto* c : _components) c->loop();
}
