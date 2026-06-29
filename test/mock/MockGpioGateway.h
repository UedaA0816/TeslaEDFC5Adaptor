#pragma once

#include "shared/IGpioGateway.h"

struct MockGpioGateway : public IGpioGateway {
    void begin() override {}
    void loop() override {}

    void setSpeedPulse(float speedKmh) override {
        lastSpeed = speedKmh;
        ++speedCount;
    }

    void setEdfc5Relay(bool e1, bool e2) override {
        ext1 = e1;
        ext2 = e2;
        ++relayCount;
    }

    void setLed(bool b, bool y) override {
        blue = b;
        yellow = y;
        ++ledCount;
    }

    // 記録
    float lastSpeed = -1.0f;
    int speedCount = 0;

    bool ext1 = false, ext2 = false;
    int relayCount = 0;

    bool blue = false, yellow = false;
    int ledCount = 0;
};
