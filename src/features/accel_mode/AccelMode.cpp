// ============================================================
// features/accel_mode/AccelMode.cpp
// ============================================================

#include "AccelMode.h"

void AccelMode::onAccelModeUpdate(AccelModeType mode) {
    applyMode(mode);
}

// ── 加速モード → EDFC5 外部入力 / LED ─────────────────────────
// EXT1 ON: 外部入力1（ハード方向 / Sport）
// EXT2 ON: 外部入力2（ソフト方向 / Chill）
// 両 OFF : Standard（EDFC5 のマニュアル設定値に従う）
void AccelMode::applyMode(AccelModeType mode) {
    switch (mode) {
        case AccelModeType::CHILL:
            _gpio.setEdfc5Relay(false, true);   // EXT2 ON → ソフト
            _gpio.setLed(true, false);          // 青点灯
            Serial.println("[Mode] Chill → EXT2 ON");
            break;

        case AccelModeType::STANDARD:
            _gpio.setEdfc5Relay(false, false);  // 両 OFF → マニュアル値
            _gpio.setLed(true, true);           // 両点灯
            Serial.println("[Mode] Standard → リレー OFF");
            break;

        case AccelModeType::SPORT:
            _gpio.setEdfc5Relay(true, false);   // EXT1 ON → ハード
            _gpio.setLed(false, true);          // 黄点灯
            Serial.println("[Mode] Sport → EXT1 ON");
            break;

        default:
            _gpio.setEdfc5Relay(false, false);
            _gpio.setLed(false, false);
            break;
    }
}
