// ============================================================
// gateway/can/CanGateway.cpp
// ============================================================

#include "CanGateway.h"

CanGateway::CanGateway(gpio_num_t txPin, gpio_num_t rxPin, gpio_num_t rsPin)
    : _txPin(txPin), _rxPin(rxPin), _rsPin(rsPin) {}

bool CanGateway::begin() {
    // トランシーバーを高速モードに設定（RS = LOW）
    pinMode(_rsPin, OUTPUT);
    digitalWrite(_rsPin, LOW);

    twai_general_config_t g = TWAI_GENERAL_CONFIG_DEFAULT(
        _txPin, _rxPin, TWAI_MODE_LISTEN_ONLY
    );
    twai_timing_config_t  t = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t  f = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g, &t, &f) != ESP_OK) {
        Serial.println("[CAN] ドライバーインストール失敗");
        return false;
    }
    if (twai_start() != ESP_OK) {
        Serial.println("[CAN] 起動失敗");
        return false;
    }
    Serial.println("[CAN] 起動完了 ✓ 500kbps / Listen Only");
    return true;
}

bool CanGateway::addListener(ICanListener* listener) {
    if (_listenerCount >= MAX_LISTENERS || !listener) return false;
    _listeners[_listenerCount++] = listener;
    return true;
}

void CanGateway::update() {
    twai_message_t msg;
    if (twai_receive(&msg, pdMS_TO_TICKS(5)) != ESP_OK) return;

    _rxCount++;
    updateRxRate();

    // 全メッセージをシリアルダンプ（Phase 1: 加速モードID特定用）
    Serial.printf("%8lu | 0x%03X |", millis(), msg.identifier);
    for (int i = 0; i < msg.data_length_code; i++)
        Serial.printf(" %02X", msg.data[i]);
    Serial.println();

    // 既知シグナルのパースと通知
    if (msg.identifier == CAN_SPEED_ID) {
        float kmh = parseVehicleSpeed(msg);
        if (fabsf(kmh - _signals.vehicleSpeedKmh) > 0.3f) {
            _signals.vehicleSpeedKmh = kmh;
            _signals.lastSpeedMs     = millis();
            notifySpeed(kmh);
        }
    }

    // 加速モード（Phase 1 で CAN_MODE_ID を確定後に有効化）
    if (CAN_MODE_ID != 0x000 && msg.identifier == CAN_MODE_ID) {
        AccelModeType mode = parseAccelMode(msg);
        if (mode != _signals.accelMode) {
            _signals.accelMode  = mode;
            _signals.lastModeMs = millis();
            notifyMode(mode);
        }
    }
}

// ── 車速パース ───────────────────────────────────────────────
// DI_vehicleSpeed: bits[12:23], scale=0.08, offset=-40.0, unit=kph
float CanGateway::parseVehicleSpeed(const twai_message_t& msg) {
    if (msg.data_length_code < 3) return 0.0f;
    uint16_t raw = (uint16_t)(msg.data[1] | (msg.data[2] << 8));
    raw = (raw >> 4) & 0x0FFF;
    float kmh = raw * 0.08f - 40.0f;
    return (kmh < 0.0f) ? 0.0f : kmh;
}

// ── 加速モードパース（Phase 1 で仕様確定後に実装）──────────
AccelModeType CanGateway::parseAccelMode(const twai_message_t& msg) {
    // TODO: Phase 1 ダンプで CAN_MODE_ID・ビット位置を特定して実装
    // 暫定: data[0] の下位2ビットで判定（要検証）
    switch (msg.data[0] & 0x03) {
        case 0x01: return AccelModeType::CHILL;
        case 0x02: return AccelModeType::STANDARD;
        case 0x03: return AccelModeType::SPORT;
        default:   return AccelModeType::UNKNOWN;
    }
}

// ── リスナー通知 ─────────────────────────────────────────────
void CanGateway::notifySpeed(float kmh) {
    for (uint8_t i = 0; i < _listenerCount; i++)
        _listeners[i]->onSpeedUpdate(kmh);
}

void CanGateway::notifyMode(AccelModeType mode) {
    Serial.printf("[CAN] モード変更: %s\n", accelModeStr(mode));
    for (uint8_t i = 0; i < _listenerCount; i++)
        _listeners[i]->onAccelModeUpdate(mode);
}

// ── 受信レート計算（1秒ごとに更新）──────────────────────────
void CanGateway::updateRxRate() {
    uint32_t now = millis();
    if (now - _lastRateMs >= 1000) {
        _signals.canRxRate = (uint16_t)_rxCount;
        _rxCount    = 0;
        _lastRateMs = now;
    }
}
