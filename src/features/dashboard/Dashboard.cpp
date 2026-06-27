// ============================================================
// features/dashboard/Dashboard.cpp
// ============================================================

#include "Dashboard.h"

void Dashboard::onSpeedUpdate(float speedKmh) {
    _cache.vehicleSpeedKmh = speedKmh;
    broadcast();
}

void Dashboard::onAccelModeUpdate(AccelModeType mode) {
    _cache.accelMode = mode;
    broadcast();
}

void Dashboard::broadcast() {
    _wifi.broadcastSignals(_cache);
}
