// ============================================================
// native テスト用 Arduino.h スタブ
// TeslaSignals.h の stdint 型と AccelMode.cpp の Serial を解決する。
// 本番コードは無改変。
// ============================================================
#pragma once

#include <cstdint>
#include <cstdio>
#include <cmath>

// Serial no-op スタブ
struct SerialStub {
    void begin(unsigned long) {}
    void print(const char*) {}
    void println(const char*) {}
    template<typename... Args>
    void printf(const char*, Args...) {}
};

static SerialStub Serial;
