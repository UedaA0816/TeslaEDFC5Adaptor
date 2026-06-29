// ============================================================
// features/FeatureInstaller.cpp
// feature ドメインの DI 登録を集約する installer。
// gateways::install() の後に呼ぶこと。
// ============================================================

#include <memory>

#include "DIContainer.h"
#include "features/FeatureInstaller.h"
#include "gateway/GatewayInstaller.h"

#include "features/accel_mode/AccelMode.h"
#include "features/speed_pulse/SpeedPulse.h"

void features::install(DIContainer& c) {
    auto gw = gateways::buildGatewaySet(c);
    c.AddComponent<SpeedPulse>(
        [gw] { return std::make_shared<SpeedPulse>(gw); }
    );
    c.AddComponent<AccelMode>(
        [gw] { return std::make_shared<AccelMode>(gw); }
    );
}
