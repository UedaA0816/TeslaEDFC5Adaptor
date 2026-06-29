// ============================================================
// app/App.cpp
// ============================================================

#include <Arduino.h>

#include "app/App.h"

void App::setup() {
    Serial.begin(115200);
    delay(1500);
    Serial.println("\n=== RejsaCAN Tesla CAN × EDFC5 ===");

    // gateway → feature の順に begin（feature.begin() の addListener が成立する順）
    for (size_t i = 0; i < _count; ++i) _components[i]->begin();

    Serial.println("=== 初期化完了 ===");
    Serial.println("  時刻(ms)  |  CAN ID  | データ");
    Serial.println("------------|----------|-------------------");
}

void App::loop() {
    for (size_t i = 0; i < _count; ++i) _components[i]->loop();
}
