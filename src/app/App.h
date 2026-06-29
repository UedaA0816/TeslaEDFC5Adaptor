#pragma once

#include <memory>

#include "DIContainer.h"
#include "shared/IApp.h"
#include "shared/ICanGateway.h"
#include "shared/IFeature.h"
#include "shared/IGpioGateway.h"

#include "features/accel_mode/AccelMode.h"
#include "features/speed_pulse/SpeedPulse.h"

// ============================================================
// orchestration（board 非依存）
// 合成ルートが gateway を登録済みの DIContainer を渡す。
// feature の構築・gateway の begin/update 駆動を担う。
// ============================================================

class App : public IApp {
  public:
    explicit App(DIContainer& container);

    void setup() override;
    void loop() override;

  private:
    DIContainer& _container;

    // 宣言順 = 初期化順。feature は構築時に DIContainer から gateway を Resolve する。
    SpeedPulse _speedPulse;
    AccelMode  _accelMode;
    IFeature*  _features[2];

    std::shared_ptr<ICanGateway>  _can;
    std::shared_ptr<IGpioGateway> _gpio;
};
