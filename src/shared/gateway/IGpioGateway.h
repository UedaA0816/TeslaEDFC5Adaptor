#pragma once

#include "shared/IGateway.h"

class IGpioGateway: public IGateway {
  public:
    virtual ~IGpioGateway() = default;
    virtual void setSpeedPulse(float speedKmh) = 0;
    virtual void setEdfc5Relay(bool ext1, bool ext2) = 0;
    virtual void setLed(bool blue, bool yellow) = 0;
};