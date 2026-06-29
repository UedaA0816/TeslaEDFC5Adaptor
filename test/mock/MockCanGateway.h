#pragma once

#include "shared/ICanGateway.h"

struct MockCanGateway : public ICanGateway {
    void begin() override {}
    void loop() override {}

    bool addListener(ICanListener* listener) override {
        lastListener = listener;
        ++addListenerCount;
        return true;
    }

    // 記録
    ICanListener* lastListener = nullptr;
    int addListenerCount = 0;
};
