// ============================================================
// gateway/gpio/GpioGateway.cpp
// ============================================================

#include "GpioGateway.h"

void GpioGateway::begin() {
    // LED
    pinMode(LED_BLUE_PIN,   OUTPUT);
    pinMode(LED_YELLOW_PIN, OUTPUT);
    digitalWrite(LED_BLUE_PIN,   LOW);
    digitalWrite(LED_YELLOW_PIN, LOW);

    // リレー（Phase 3: EDFC5連携時に有効化）
    pinMode(RELAY_EXT1_PIN, OUTPUT);
    pinMode(RELAY_EXT2_PIN, OUTPUT);
    digitalWrite(RELAY_EXT1_PIN, LOW);
    digitalWrite(RELAY_EXT2_PIN, LOW);

    // 車速PWM（Phase 3: EDFC5連携時に有効化）
    ledcSetup(PWM_CHANNEL, PWM_FREQ_HZ, PWM_BITS);
    ledcAttachPin(SPEED_PULSE_PIN, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, 0);  // 初期は停止

    Serial.println("[GPIO] 初期化完了");
}

// ── 車速パルス出力 ────────────────────────────────────────────
// EDFC5 の車速入力端子に接続。km/h → PWM周波数に変換して出力。
// パルス数/km は EDFC5 マニュアルで確認して speedToFreq() を調整すること。
void GpioGateway::setSpeedPulse(float speedKmh) {
    if (speedKmh <= 0.0f) {
        ledcWrite(PWM_CHANNEL, 0);
        return;
    }
    uint32_t freq = speedToFreq(speedKmh);
    ledcChangeFrequency(PWM_CHANNEL, freq, PWM_BITS);
    ledcWrite(PWM_CHANNEL, 128);  // デューティ 50%
}

// ── EDFC5 リレー制御 ──────────────────────────────────────────
// ext1=true: 外部入力1 ON（Sport / ハード方向）
// ext2=true: 外部入力2 ON（Chill / ソフト方向）
// 両方 false: Standard（マニュアル設定値）
void GpioGateway::setEdfc5Relay(bool ext1, bool ext2) {
    digitalWrite(RELAY_EXT1_PIN, ext1 ? HIGH : LOW);
    digitalWrite(RELAY_EXT2_PIN, ext2 ? HIGH : LOW);
}

// ── ステータスLED ────────────────────────────────────────────
void GpioGateway::setLed(bool blue, bool yellow) {
    digitalWrite(LED_BLUE_PIN,   blue   ? HIGH : LOW);
    digitalWrite(LED_YELLOW_PIN, yellow ? HIGH : LOW);
}

// ── 速度 → PWM周波数変換 ──────────────────────────────────────
// 例: EDFC5 が 4000パルス/km を期待する場合
//   100 km/h = 100,000 m/h = 27.78 m/s
//   27.78 m/s × 4000 pulse/km × (1/1000) = 111 Hz
// ⚠️ EDFC5 マニュアルで実際の値を確認してから調整すること
uint32_t GpioGateway::speedToFreq(float kmh) {
    const float PULSES_PER_KM = 4000.0f;  // TODO: EDFC5仕様確認後に調整
    float mps  = kmh / 3.6f;
    float freq = mps * PULSES_PER_KM / 1000.0f;
    return (uint32_t)freq;
}
