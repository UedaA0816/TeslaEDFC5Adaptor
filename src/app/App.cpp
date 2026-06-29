// ============================================================
// app/App.cpp
// ============================================================

#include <Arduino.h>

#include "app/App.h"

App::App(DIContainer& container)
    : _container(container),
      _speedPulse(container),
      _accelMode(container),
      _features{ &_speedPulse, &_accelMode },
      _can(container.Resolve<ICanGateway>()),
      _gpio(container.Resolve<IGpioGateway>()) {}

void App::setup() {
    Serial.begin(115200);
    delay(1500);
    Serial.println("\n=== RejsaCAN Tesla CAN × EDFC5 ===");

    _gpio->begin();
    _can->begin();

    // feature が自身を CAN リスナーとして登録する
    for (IFeature* f : _features) f->setup();

    Serial.println("=== 初期化完了 ===");
    Serial.println("  時刻(ms)  |  CAN ID  | データ");
    Serial.println("------------|----------|-------------------");
}

void App::loop() {
    _can->update();
    for (IFeature* f : _features) f->loop();
}
