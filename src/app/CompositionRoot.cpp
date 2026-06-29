// ============================================================
// app/CompositionRoot.cpp
// ★ここが唯一の board 配線ファイル：ピン定数・具体 gateway・DI 登録を集約。
//   board を差し替える場合はこのファイルだけを差し替えればよい。
// ============================================================

#include <Arduino.h>

#include <memory>

#include "app/App.h"
#include "app/CompositionRoot.h"
#include "DIContainer.h"

#include "gateway/can/CanGateway.h"
#include "gateway/gpio/GpioGateway.h"

namespace {
// RejsaCAN-ESP32-S3 v3.4 CAN ピン: TX=GPIO12 / RX=GPIO13 / RS=GPIO38
constexpr gpio_num_t CAN_TX = GPIO_NUM_12;
constexpr gpio_num_t CAN_RX = GPIO_NUM_13;
constexpr gpio_num_t CAN_RS = GPIO_NUM_38;
}  // namespace

// TU 跨ぎの静的初期化順問題を避けるため function-local static（Meyers singleton）で構築。
// container（gateway 登録済み）→ App の順に初期化されるため feature の Resolve が成功する。
IApp& appInstance() {
    static DIContainer container = [] {
        DIContainer c;
        c.Register<ICanGateway>(
            Lifecycle::Singleton,
            [] { return std::make_shared<CanGateway>(CAN_TX, CAN_RX, CAN_RS); }
        );
        c.Register<IGpioGateway, GpioGateway>(Lifecycle::Singleton);
        return c;
    }();
    static App app(container);
    return app;
}
