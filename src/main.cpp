// ============================================================
// main.cpp
// 責務: 初期化・依存注入のみ。ビジネスロジックを持たない。
//
// 依存の方向:
//   main → gateway（生成）
//   main → features（生成・gateway注入）
//   features → shared（インターフェース経由）
//   features → gateway（インターフェース経由）
// ============================================================

#include <Arduino.h>

// Gateway
#include "gateway/can/CanGateway.h"
#include "gateway/gpio/GpioGateway.h"
#include "gateway/wifi/WifiGateway.h"

// Features
#include "features/dashboard/Dashboard.h"
#include "features/speed_pulse/SpeedPulse.h"
#include "features/accel_mode/AccelMode.h"

// ── RejsaCAN v3.4 確定ピン ─────────────────────────────────
static constexpr gpio_num_t PIN_CAN_TX = GPIO_NUM_12;
static constexpr gpio_num_t PIN_CAN_RX = GPIO_NUM_13;
static constexpr gpio_num_t PIN_CAN_RS = GPIO_NUM_38;

// WiFi 起動ボタン（空きGPIOから選択）
static constexpr uint8_t WIFI_BUTTON_PIN = 16;

// ── オブジェクト生成 ──────────────────────────────────────
CanGateway  canGateway(PIN_CAN_TX, PIN_CAN_RX, PIN_CAN_RS);
GpioGateway gpioGateway;
WifiGateway wifiGateway;

// Features（Gatewayを注入）
Dashboard  dashboard(wifiGateway);   // WiFiに出力
SpeedPulse speedPulse(gpioGateway);  // GPIOに出力
AccelMode  accelMode(gpioGateway);   // GPIOに出力

// ── setup ─────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    delay(1500);
    Serial.println("\n=== RejsaCAN Tesla CANブリッジ ===");

    // Gateway 初期化
    gpioGateway.begin();
    wifiGateway.begin();

    // 起動確認LED（交互点滅）
    for (int i = 0; i < 4; i++) {
        gpioGateway.setLed(i % 2 == 0, i % 2 == 1);
        delay(150);
    }
    gpioGateway.setLed(false, false);

    // WiFi ボタン
    pinMode(WIFI_BUTTON_PIN, INPUT_PULLUP);

    // CAN 初期化 + リスナー登録（依存注入）
    canGateway.begin();
    canGateway.addListener(&dashboard);
    canGateway.addListener(&speedPulse);
    canGateway.addListener(&accelMode);

    Serial.println("=== 初期化完了 ===");
    Serial.println("  時刻(ms)  |  CAN ID  | データ");
    Serial.println("------------|----------|-------------------");
}

// ── loop ──────────────────────────────────────────────────
void loop() {
    // CAN受信・パース・リスナー通知
    canGateway.update();

    // WiFiオンデマンド起動（ボタン長押し不要・単押しで5分間ON）
    static bool lastBtn = HIGH;
    bool nowBtn = digitalRead(WIFI_BUTTON_PIN);
    if (lastBtn == HIGH && nowBtn == LOW) {
        wifiGateway.isActive() ? wifiGateway.stopAP() : wifiGateway.startAP();
    }
    lastBtn = nowBtn;

    // WiFiタイムアウト管理
    wifiGateway.update();
}
