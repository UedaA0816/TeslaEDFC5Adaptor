// ============================================================
// features/speed_pulse/SpeedPulse.cpp
// ============================================================

#include "SpeedPulse.h"

void SpeedPulse::onSpeedUpdate(float speedKmh) {
    _gpio.setSpeedPulse(speedKmh);
}

void SpeedPulse::onAccelModeUpdate(AccelModeType mode) {
    (void)mode;
}
