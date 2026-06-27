#pragma once

#include <Arduino.h>

#include "shared/ICanGateway.h"
#include "shared/IGateway.h"
#include "shared/Types.h"

class Dashboard : public ICanListener, public IFeature {
  public:
    explicit Dashboard(DIContainer& diContainer)
        : IFeature(diContainer), _wifi(*diContainer.Resolve<IWifiGateway>()) {}
    void setup() override {}
    void loop() override {}
    void onSpeedUpdate(float speedKmh);
    void onAccelModeUpdate(AccelModeType mode);

  private:
    void broadcast();

    TeslaSignals _cache{};
    IWifiGateway& _wifi;
};
