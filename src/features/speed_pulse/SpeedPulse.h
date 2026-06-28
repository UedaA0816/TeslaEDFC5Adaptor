#pragma once

#include "DIContainer.h"
#include "shared/ICanGateway.h"
#include "shared/ICanListener.h"
#include "shared/IFeature.h"
#include "shared/IGpioGateway.h"

// ============================================================
// 車速 → EDFC5 車速パルス変換 feature
// CAN の車速更新を受けて GPIO の PWM パルスへ橋渡しするだけ。
// ============================================================

class SpeedPulse : public ICanListener, public IFeature {
  public:
    explicit SpeedPulse(DIContainer& diContainer)
        : IFeature(diContainer), _gpio(*diContainer.Resolve<IGpioGateway>()) {}

    void setup() override;
    void loop() override {}
    void onSpeedUpdate(float speedKmh) override;

  private:
    IGpioGateway& _gpio;
};
