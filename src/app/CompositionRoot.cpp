// ============================================================
// app/CompositionRoot.cpp
// ★ここが唯一の board 配線ファイル：ピン定数・具体 gateway・DI 登録・
//   feature への注入・起動順を集約。board 差し替え時はこのファイルだけ差し替える。
// ============================================================

#include <Arduino.h>

#include <memory>

#include "app/App.h"
#include "app/CompositionRoot.h"
#include "DIContainer.h"

#include "shared/ILifeCycle.h"
#include "gateway/can/CanGateway.h"
#include "gateway/gpio/GpioGateway.h"
#include "features/accel_mode/AccelMode.h"
#include "features/speed_pulse/SpeedPulse.h"

namespace {
// RejsaCAN-ESP32-S3 v3.4 CAN ピン: TX=GPIO12 / RX=GPIO13 / RS=GPIO38
constexpr gpio_num_t CAN_TX = GPIO_NUM_12;
constexpr gpio_num_t CAN_RX = GPIO_NUM_13;
constexpr gpio_num_t CAN_RS = GPIO_NUM_38;
}  // namespace

// TU 跨ぎの静的初期化順問題を避けるため function-local static で構築。
// DIContainer で gateway singleton を Resolve し、feature へコンストラクタ注入する。
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

    static auto gpio = container.Resolve<IGpioGateway>();
    static auto can  = container.Resolve<ICanGateway>();

    // feature は必要な gateway IF だけを注入で受け取る
    static SpeedPulse speedPulse(*can, *gpio);
    static AccelMode  accelMode(*can, *gpio);

    // 起動・駆動順：gateway → feature（feature.begin() の addListener が成立する順）
    static ILifeCycle* components[] = {
        gpio.get(),
        can.get(),
        &speedPulse,
        &accelMode,
    };

    static App app(components, sizeof(components) / sizeof(components[0]));
    return app;
}
