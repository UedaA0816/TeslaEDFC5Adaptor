#pragma once

#include <Arduino.h>

#include "shared/ICanGateway.h"
#include "shared/IGateway.h"

class SpeedPulse : public ICanListener, public IFeature {
  public:
    explicit SpeedPulse(DIContainer& diContainer)
        : IFeature(diContainer), _gpio(*diContainer.Resolve<IGpioGateway>()) {}
    void setup() override {}
    void loop() override {}
    void onSpeedUpdate(float speedKmh) override;
    void onAccelModeUpdate(AccelModeType mode) override;

  private:
    IGpioGateway& _gpio;
};
