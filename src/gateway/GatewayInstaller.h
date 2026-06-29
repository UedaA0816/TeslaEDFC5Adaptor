#pragma once

#include <driver/gpio.h>

#include "shared/GatewaySet.h"

class DIContainer;

namespace gateways {

struct CanPins {
    gpio_num_t tx;
    gpio_num_t rx;
    gpio_num_t rs;
};

void install(DIContainer& container, const CanPins& canPins);

// install() 後に呼ぶ。登録済み gateway を Resolve して GatewaySet を構築する。
GatewaySet buildGatewaySet(DIContainer& container);

}  // namespace gateways
