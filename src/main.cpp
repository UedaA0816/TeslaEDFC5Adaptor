// ============================================================
// main.cpp
// 責務: 初期化・依存注入のみ。ビジネスロジックを持たない。
//
// 依存の方向:
//   main → feature ←(DI)- gateway ← shared
// ============================================================

#include <Arduino.h>

#include "DIContainer.h"

// Features
#include "features/dashboard/Dashboard.h"
#include "features/speed_pulse/SpeedPulse.h"
#include "features/accel_mode/AccelMode.h"

// Gateways
#include "gateway/can/CanGateway.h"
#include "gateway/gpio/GpioGateway.h"
#include "gateway/wifi/WifiGateway.h"

DIContainer container;

namespace {
struct ServiceRegistrar {
    ServiceRegistrar() {
        container.Register<ICanGateway>(
            Lifecycle::Singleton,
            []() {
                return std::make_shared<CanGateway>(GPIO_NUM_17, GPIO_NUM_18, GPIO_NUM_5);
            }
        );
        container.Register<IWifiGateway, WifiGateway>(Lifecycle::Singleton);
        container.Register<IGpioGateway, GpioGateway>(Lifecycle::Singleton);
    }
} registrar;

Dashboard dashboard(container);
SpeedPulse speedPulse(container);
AccelMode accelMode(container);

IFeature* features[] = {
    &dashboard,
    &speedPulse,
    &accelMode,
};
}
// ── setup ─────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    delay(1500);
    Serial.println("\n=== RejsaCAN Tesla CANブリッジ ===");

    for (size_t i = 0; i < (sizeof(features) / sizeof(features[0])); ++i) {
        features[i]->setup();
    }

    Serial.println("=== 初期化完了 ===");
    Serial.println("  時刻(ms)  |  CAN ID  | データ");
    Serial.println("------------|----------|-------------------");
}

// ── loop ──────────────────────────────────────────────────
void loop() {
    for (size_t i = 0; i < (sizeof(features) / sizeof(features[0])); ++i) {
        features[i]->loop();
    }
}
