#pragma once

#include <Arduino.h>

#include "shared/IGateway.h"

class ICanGateway : public IGateway {
  public:
    virtual ~ICanGateway() = default;
    virtual void start() override = 0;
    virtual bool addListener(ICanListener* listener) = 0;
    virtual void update() override = 0;
};
