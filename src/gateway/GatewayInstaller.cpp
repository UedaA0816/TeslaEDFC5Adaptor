// ============================================================
// gateway/GatewayInstaller.cpp
// gateway ドメインの DI 登録を集約する installer。
// ============================================================

#include <memory>

#include "DIContainer.h"
#include "gateway/GatewayInstaller.h"
#include "shared/ICanGateway.h"
#include "shared/IGpioGateway.h"

#include "gateway/can/CanGateway.h"
#include "gateway/gpio/GpioGateway.h"

void gateways::install(DIContainer& c, const CanPins& p) {
    // 登録順 = begin/loop 駆動順。GPIO → CAN の順に初期化する。
    c.AddComponent<IGpioGateway, GpioGateway>();
    c.AddComponent<ICanGateway>(
        [p] { return std::make_shared<CanGateway>(p.tx, p.rx, p.rs); }
    );
}

GatewaySet gateways::buildGatewaySet(DIContainer& c) {
    return { *c.Resolve<ICanGateway>(), *c.Resolve<IGpioGateway>() };
}
