// ============================================================
// features/speed_pulse/SpeedPulse.cpp
// ============================================================

#include "SpeedPulse.h"

void SpeedPulse::setup() {
    diContainer().Resolve<ICanGateway>()->addListener(this);
}

void SpeedPulse::onSpeedUpdate(float speedKmh) {
    _gpio.setSpeedPulse(speedKmh);
}
