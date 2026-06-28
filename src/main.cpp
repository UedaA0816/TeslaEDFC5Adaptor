// ============================================================
// main.cpp
// 責務: 初期化・依存注入のみ。ビジネスロジックを持たない。
//
// 依存の方向:
//   main → feature ←(DI)- gateway ← shared
// ============================================================

#include <Arduino.h>

#include <memory>

#include "DIContainer.h"

// Features
#include "features/speed_pulse/SpeedPulse.h"
#include "features/accel_mode/AccelMode.h"

// Gateways
#include "gateway/can/CanGateway.h"
#include "gateway/gpio/GpioGateway.h"

// Shared
#include "shared/ICanGateway.h"
#include "shared/IGpioGateway.h"
#include "shared/IFeature.h"

DIContainer container;

namespace {
// RejsaCAN-ESP32-S3 v3.4 CAN ピン: TX=GPIO12 / RX=GPIO13 / RS=GPIO38
constexpr gpio_num_t CAN_TX = GPIO_NUM_12;
constexpr gpio_num_t CAN_RX = GPIO_NUM_13;
constexpr gpio_num_t CAN_RS = GPIO_NUM_38;

struct ServiceRegistrar {
    ServiceRegistrar() {
        container.Register<ICanGateway>(
            Lifecycle::Singleton,
            []() { return std::make_shared<CanGateway>(CAN_TX, CAN_RX, CAN_RS); }
        );
        container.Register<IGpioGateway, GpioGateway>(Lifecycle::Singleton);
    }
} registrar;

SpeedPulse speedPulse(container);
AccelMode  accelMode(container);

IFeature* features[] = {
    &speedPulse,
    &accelMode,
};

std::shared_ptr<ICanGateway>  canGateway;
std::shared_ptr<IGpioGateway> gpioGateway;
}  // namespace

// ── setup ─────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    delay(1500);
    Serial.println("\n=== RejsaCAN Tesla CAN × EDFC5 ===");

    canGateway  = container.Resolve<ICanGateway>();
    gpioGateway = container.Resolve<IGpioGateway>();

    gpioGateway->begin();
    canGateway->begin();

    // feature が自身を CAN リスナーとして登録する
    for (IFeature* f : features) f->setup();

    Serial.println("=== 初期化完了 ===");
    Serial.println("  時刻(ms)  |  CAN ID  | データ");
    Serial.println("------------|----------|-------------------");
}

// ── loop ──────────────────────────────────────────────────
void loop() {
    canGateway->update();
    for (IFeature* f : features) f->loop();
}
