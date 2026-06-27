// ============================================================
// features/speed_pulse/SpeedPulse.cpp
// ============================================================

#include "SpeedPulse.h"

void SpeedPulse::onSpeedUpdate(float speedKmh) {
    _gpio.setSpeedPulse(speedKmh);
}
