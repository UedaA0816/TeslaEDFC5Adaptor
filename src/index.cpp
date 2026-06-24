/**
 * RejsaCAN-ESP32-S3 v3.4
 * Lチカ検証スケッチ - Phase 1
 * ================================
 * 目的：
 *   1. Tesla CAN busの全メッセージをシリアルにダンプ
 *   2. 車速（DI_vehicleSpeed）をパースしてLED表示
 *   3. 加速モードのCAN IDを特定するためのログ取得
 *
 * 接続：
 *   RejsaCAN-ESP32-S3 → OBD2ケーブル → Tesla 26ピンアダプター → X181コネクター
 *
 * ピン情報ソース：
 *   https://github.com/MagnusThome/RejsaCAN-ESP32 (公式README v3.4)
 */

#include <Arduino.h>
#include <driver/twai.h>

// =====================================================
// ピン設定（RejsaCAN v3.4 公式GitHubより確定）
// =====================================================
#define TWAI_TX_PIN     GPIO_NUM_12  // CAN_TX → SN65HVD230 ✓
#define TWAI_RX_PIN     GPIO_NUM_13  // CAN_RX ← SN65HVD230 ✓（スリープ解除トリガーにも使用）
#define CAN_RS_PIN      GPIO_NUM_38  // トランシーバー スタンバイ制御 ✓
                                     //   LOW  = 高速モード（通常使用）
                                     //   HIGH = スタンバイ（省電力）
#define HI_DRIVER_PIN   GPIO_NUM_21  // スイッチド3.3V出力制御 ✓（外部センサー給電用）
#define SENSE_V_PIN     GPIO_NUM_9   // 車両電圧モニタリング（アナログ） ✓

// 表面実装LED（公式GitHubより確定）
// ※ 青LEDが在庫切れの場合、赤LEDで代替されているボードあり
#define LED_BLUE_PIN    GPIO_NUM_10  // 青LED（または赤） ✓
#define LED_YELLOW_PIN  GPIO_NUM_11  // 黄LED ✓

// =====================================================
// Tesla Model 3 CAN 既知シグナル
// =====================================================
#define ID_DI_SPEED     0x257   // DI_vehicleSpeed（確認済み）

// 加速モード候補ID（今回の検証で特定する）
// シリアルログで変化するIDを探す
#define ID_UNKNOWN_MODE 0x000   // 未特定（検証後に更新）

// =====================================================
// グローバル変数
// =====================================================
float    g_speed       = 0.0;
uint8_t  g_accelMode   = 0;    // 0=Standard, 1=Chill, 2=Sport（仮）
bool     g_canReady    = false;
uint32_t g_lastLedMs   = 0;
bool     g_ledState    = false;

// =====================================================
// TWAI（CAN）初期化
// =====================================================
void initCAN() {
  // トランシーバーを高速モードに設定（LOW = アクティブ）
  pinMode(CAN_RS_PIN, OUTPUT);
  digitalWrite(CAN_RS_PIN, LOW);

  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
    TWAI_TX_PIN,
    TWAI_RX_PIN,
    TWAI_MODE_LISTEN_ONLY   // 読み取り専用（書き込みなし・安全）
  );
  twai_timing_config_t  t_config = TWAI_TIMING_CONFIG_500KBITS();
  twai_filter_config_t  f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  esp_err_t err;

  err = twai_driver_install(&g_config, &t_config, &f_config);
  if (err != ESP_OK) {
    Serial.printf("[CAN] ドライバーエラー: %s\n", esp_err_to_name(err));
    return;
  }

  err = twai_start();
  if (err != ESP_OK) {
    Serial.printf("[CAN] 起動エラー: %s\n", esp_err_to_name(err));
    return;
  }

  Serial.println("[CAN] 起動成功 ✓ 500kbps / Listen Only");
  g_canReady = true;
}

// =====================================================
// 車速パース（DI_vehicleSpeed, ID: 0x257）
// bits[12:23], scale=0.08, offset=-40.0, unit=kph
// =====================================================
float parseVehicleSpeed(const twai_message_t &msg) {
  if (msg.data_length_code < 3) return 0.0;
  uint16_t raw = (uint16_t)(msg.data[1] | (msg.data[2] << 8));
  raw = (raw >> 4) & 0x0FFF;
  float speed = raw * 0.08f - 40.0f;
  return (speed < 0.0f) ? 0.0f : speed;
}

// =====================================================
// LED更新
// モード判明前は車速でシンプルに表示
// =====================================================
void updateLED() {
  uint32_t now = millis();

  if (g_speed <= 0.5f) {
    // 停止中：両LED OFF
    digitalWrite(LED_BLUE_PIN,   LOW);
    digitalWrite(LED_YELLOW_PIN, LOW);

  } else if (g_speed < 60.0f) {
    // 低速（〜60km/h）：青ゆっくり点滅
    if (now - g_lastLedMs > 800) {
      g_ledState = !g_ledState;
      digitalWrite(LED_BLUE_PIN,   g_ledState ? HIGH : LOW);
      digitalWrite(LED_YELLOW_PIN, LOW);
      g_lastLedMs = now;
    }

  } else {
    // 高速（60km/h〜）：黄はやく点滅
    if (now - g_lastLedMs > 300) {
      g_ledState = !g_ledState;
      digitalWrite(LED_BLUE_PIN,   LOW);
      digitalWrite(LED_YELLOW_PIN, g_ledState ? HIGH : LOW);
      g_lastLedMs = now;
    }
  }
}

// =====================================================
// CAN全ダンプ（シリアルモニターで確認）
// フォーマット: Timestamp, ID, DLC, Bytes...
// =====================================================
void dumpMessage(const twai_message_t &msg) {
  Serial.printf("%8lu | 0x%03X | %d |",
    millis(), msg.identifier, msg.data_length_code);
  for (int i = 0; i < msg.data_length_code; i++) {
    Serial.printf(" %02X", msg.data[i]);
  }
  Serial.println();
}

// =====================================================
// Setup
// =====================================================
void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("\n=============================");
  Serial.println(" RejsaCAN Lチカ検証 Phase 1");
  Serial.println("=============================");

  // LED初期化
  pinMode(LED_BLUE_PIN,   OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);

  // 起動確認：交互点滅
  for (int i = 0; i < 4; i++) {
    digitalWrite(LED_BLUE_PIN,   i % 2 == 0 ? HIGH : LOW);
    digitalWrite(LED_YELLOW_PIN, i % 2 == 1 ? HIGH : LOW);
    delay(150);
  }
  digitalWrite(LED_BLUE_PIN,   LOW);
  digitalWrite(LED_YELLOW_PIN, LOW);

  // CAN初期化
  initCAN();

  // ヘッダー出力
  Serial.println("\n  時刻(ms) |  CAN ID | DLC | データ");
  Serial.println("-----------|---------|-----|-------------------");
}

// =====================================================
// Loop
// =====================================================
void loop() {
  // LED更新（ノンブロッキング）
  updateLED();

  if (!g_canReady) return;

  // CAN受信（10ms待機）
  twai_message_t msg;
  esp_err_t err = twai_receive(&msg, pdMS_TO_TICKS(10));
  if (err != ESP_OK) return;

  // 全メッセージダンプ
  dumpMessage(msg);

  // 既知シグナルの処理
  switch (msg.identifier) {

    case ID_DI_SPEED: {
      float newSpeed = parseVehicleSpeed(msg);
      if (abs(newSpeed - g_speed) > 0.5f) {
        g_speed = newSpeed;
        Serial.printf(">>> 車速: %.1f km/h\n", g_speed);
      }
      break;
    }

    // 加速モード候補（検証後にIDをここに追加）
    // case 0x???:
    //   Serial.printf(">>> モード候補: %02X %02X\n",
    //     msg.data[0], msg.data[1]);
    //   break;

    default:
      break;
  }
}