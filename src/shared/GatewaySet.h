#pragma once

#include "shared/ICanGateway.h"
#include "shared/IGpioGateway.h"

// ============================================================
// 全 gateway 参照のバンドル。feature はコンストラクタで受け取り、
// 使うものだけメンバに引き出す。
// gateway 追加時はここに1行追加するだけ。
// ============================================================

struct GatewaySet {
    ICanGateway&  can;
    IGpioGateway& gpio;
};
