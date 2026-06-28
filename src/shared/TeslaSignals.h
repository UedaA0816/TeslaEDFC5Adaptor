#pragma once

#include <Arduino.h>

// ============================================================
// Tesla CAN から抽出するシグナルのデータ型
// ハード非依存。gateway / feature 双方から参照される。
// ============================================================

enum class AccelModeType {
    UNKNOWN,
    CHILL,
    STANDARD,
    SPORT
};

inline const char* accelModeStr(AccelModeType mode) {
    switch (mode) {
        case AccelModeType::CHILL:    return "Chill";
        case AccelModeType::STANDARD: return "Standard";
        case AccelModeType::SPORT:    return "Sport";
        default:                      return "Unknown";
    }
}

struct TeslaSignals {
    float         vehicleSpeedKmh = 0.0f;
    AccelModeType accelMode       = AccelModeType::UNKNOWN;
    uint32_t      lastSpeedMs     = 0;
    uint32_t      lastModeMs      = 0;
    uint16_t      canRxRate       = 0;
};
