#pragma once

#include "DIContainer.h"
#include "shared/ICanGateway.h"
#include "shared/ICanListener.h"
#include "shared/IFeature.h"
#include "shared/IGpioGateway.h"
#include "shared/TeslaSignals.h"

// ============================================================
// 加速モード → EDFC5 リレー / ステータス LED 制御 feature
// CAN の加速モード更新を受けて外部入力リレーと LED を切り替える。
// ============================================================

class AccelMode : public ICanListener, public IFeature {
  public:
    explicit AccelMode(DIContainer& diContainer)
        : IFeature(diContainer), _gpio(*diContainer.Resolve<IGpioGateway>()) {}

    void setup() override;
    void loop() override {}
    void onAccelModeUpdate(AccelModeType mode) override;

  private:
    void applyMode(AccelModeType mode);
    IGpioGateway& _gpio;
};
