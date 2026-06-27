#pragma once

#include <Arduino.h>

enum class AccelModeType {
    UNKNOWN,
    CHILL,
    STANDARD,
    SPORT
};

inline const char* accelModeStr(AccelModeType mode) {
    switch (mode) {
        case AccelModeType::CHILL:
            return "Chill";
        case AccelModeType::STANDARD:
            return "Standard";
        case AccelModeType::SPORT:
            return "Sport";
        default:
            return "Unknown";
    }
}
