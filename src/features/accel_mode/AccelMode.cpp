// ============================================================
// features/accel_mode/AccelMode.cpp
// ============================================================

#include <Arduino.h>

#include "shared/features/AccelMode.h"
#include "shared/gateway/ICanGateway.h"

void AccelMode::setup() {
    auto can = _container.Resolve<ICanGateway>();
    can->addListener(filter_accel_mode(onAccelModeUpdate));
};
void AccelMode::loop() {
    // ループ処理は不要
}

void AccelMode::onAccelModeUpdate(int mode) {
    validateAccelMode(mode);
    applyMode(static_cast<AccelModeType>(mode));
}

void AccelMode::applyMode(AccelModeType mode) {
    switch (mode) {
        case AccelModeType::CHILL:
            _gpio.setEdfc5Relay(false, true);   // EXT2 ON → ソフト
            _gpio.setLed(true, false);           // 青点灯
            Serial.println("[Mode] Chill → EXT2 ON");
            break;

        case AccelModeType::STANDARD:
            _gpio.setEdfc5Relay(false, false);  // 両OFF → マニュアル値
            _gpio.setLed(true, true);            // 両点灯
            Serial.println("[Mode] Standard → リレーOFF");
            break;

        case AccelModeType::SPORT:
            _gpio.setEdfc5Relay(true, false);   // EXT1 ON → ハード
            _gpio.setLed(false, true);           // 黄点灯
            Serial.println("[Mode] Sport → EXT1 ON");
            break;

        default:
            _gpio.setEdfc5Relay(false, false);
            _gpio.setLed(false, false);
            break;
    }
}
