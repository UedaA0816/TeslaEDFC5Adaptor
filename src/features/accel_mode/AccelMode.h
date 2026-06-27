#pragma once

#include <Arduino.h>

#include "DIContainer.h"
#include "shared/features/AccelMode.h"
#include "shared/IFeature.h"
#include "shared/gateway/IGpioGateway.h"

class AccelMode : public IFeature {
  public:
    explicit AccelMode(DIContainer& diContainer)
        : IFeature(diContainer), _gpio(*diContainer.Resolve<IGpioGateway>()) {}
    void setup() override {}
    void loop() override {}

  private:
    void applyMode(AccelModeType mode);
    IGpioGateway& _gpio;
};
