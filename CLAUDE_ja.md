# Tesla CAN × TEIN EDFC5 連携システム（日本語リファレンス）

> このファイルは人間向けの詳細リファレンス。Claude が毎回読むのは英語版 `CLAUDE.md`。
> トークン節約のため、AI 向けの指示は `CLAUDE.md`（簡潔・英語）に集約し、
> 背景や経緯などの冗長な説明はこの `CLAUDE_ja.md` に置く。
> アーキテクチャ変更時は両ファイルを同期すること。

---

## プロジェクト概要

Tesla Model 3（旧式・〜2023年9月）の CAN bus を RejsaCAN-ESP32-S3 で読み取り、
TEIN EDFC5（電子制御ダンパー）を自動制御するシステム。
車速をパルス信号として EDFC5 に渡し、加速モード（Chill / Standard / Sport）に応じて
EDFC5 の外部入力リレーと減衰方向（ソフト / ハード）を切り替える。

---

## ハードウェア構成

### メインボード
- **RejsaCAN-ESP32-S3 v3.4**（Magnus Thomé 氏より直接購入）
- ESP32-S3 / 16MB Flash / 8MB PSRAM / microSD スロット搭載
- GitHub: https://github.com/MagnusThome/RejsaCAN-ESP32

### ピン配置（公式 README v3.4 準拠 / コードと一致）
```
GPIO 12  : CAN_TX
GPIO 13  : CAN_RX（スリープ解除トリガー兼用）
GPIO 38  : CAN_RS（LOW=高速モード）
GPIO 10  : LED_BLUE（※一部ボードは赤）
GPIO 11  : LED_YELLOW
GPIO 6   : RELAY_EXT1（EDFC5 外部入力1・Sport / ハード方向）← 暫定
GPIO 7   : RELAY_EXT2（EDFC5 外部入力2・Chill / ソフト方向）← 暫定
GPIO 15  : SPEED_PULSE（車速 PWM 出力）← 暫定
```

### Tesla 接続
- **車両**: Tesla Model 3 Long Range AWD（旧式・2023年9月以前）
- **CAN 接続**: フロントシート後方（センターコンソール裏）の 26 ピン診断コネクター
- **必要アダプター**: Tesla 26 ピン → OBD2 変換アダプター
- **注意**: 助手席フットウェルの X181 コネクターは電源のみ（CAN 信号なし）
- **ボーレート**: 500kbps（Tesla 公式 CD-20-17-001 確定値）
- **モード**: `TWAI_MODE_LISTEN_ONLY`（書き込み禁止）

### Tesla CAN シグナル
```
DI_vehicleSpeed : ID=0x257, bits[12:23], scale=0.08, offset=-40.0, unit=kph  ✅確定
加速モード      : ID=TBD（Phase 1 の全ダンプで特定する）
```

### TEIN EDFC5（購入予定）
- 外部入力端子1: GND プルでハード方向シフト（Sport モード連動）
- 外部入力端子2: GND プルでソフト方向シフト（Chill モード連動）
- 車速入力端子: 車速パルス信号入力（GPS キット代替）
- パルス数/km: EDFC5 マニュアル確認後に `GpioGateway.cpp` の `PULSES_PER_KM`（現状 4000）を調整

---

## ソフトウェアアーキテクチャ

### 設計方針
- **Gateway**: I/O 責任（CAN 受信・GPIO 出力）
- **Feature**: ビジネスロジックのみ（ハード依存ゼロ）
- **Shared**: 型・インターフェース定義
- **依存の方向**: Feature → Gateway（DI／インターフェース経由・逆方向なし）
- **Feature 間の依存**: なし
- **CAN 通知**: Feature が `ICanListener` を実装し `CanGateway::addListener()` で登録、コールバックを受ける
- **DI コンテナ**: Singleton 専用（`AddComponent` / `Resolve` のみ公開）

### CAN 受信の並行性
- CAN 受信は専用 FreeRTOS タスク（コア0常駐）で実行。`CanGateway::begin()` でタスクを起動し、`loop()` は空。
- listener コールバック（`onSpeedUpdate` / `onAccelModeUpdate`）はコア0のコンテキストで直接実行される。
- **⚠️ コントラクト**: 実装は非ブロッキング・短時間（GPIO 書き込み程度）に留めること。重い処理はキュー経由にする。
- GPIO 書き込みはこの経路（コア0）に一元化。コア1（Arduino loop）から GPIO を操作すると競合するため禁止。
- 将来 `_signals` 等を別コアから読む場合（旧 dashboard 機能など）はミューテックス保護が必要。

### ディレクトリ構成
```
src/
├── main.cpp                             # Arduino setup/loop → IApp に橋渡し
├── DIContainer.h                        # 簡易 DI コンテナ（Singleton 専用）
├── app/
│   ├── App.{h,cpp}                      # ILifeCycle 一括 begin/loop ランナー
│   └── CompositionRoot.{h,cpp}          # board 固有の配線（ピン・DI 登録）
├── shared/
│   ├── TeslaSignals.h                   # AccelModeType・accelModeStr()・TeslaSignals
│   ├── ICanListener.h                   # CAN 通知 IF（コア0実行コントラクト明記）
│   ├── ICanGateway.h                    # begin / addListener
│   ├── IGpioGateway.h                   # begin / setSpeedPulse / setEdfc5Relay / setLed
│   ├── ILifeCycle.h                     # begin / loop 共通 IF
│   ├── IApp.h                           # setup / loop 共通 IF
│   └── GatewaySet.h                     # ICanGateway& + IGpioGateway& バンドル
├── gateway/
│   ├── GatewayInstaller.{h,cpp}         # gateway DI 登録集約
│   ├── can/CanGateway.{h,cpp}           # TWAI 受信・パース・リスナー通知
│   └── gpio/GpioGateway.{h,cpp}         # LED・リレー・PWM 制御
└── features/
    ├── FeatureInstaller.{h,cpp}          # feature DI 登録集約
    ├── speed_pulse/SpeedPulse.h          # 車速 → PWM パルス
    └── accel_mode/AccelMode.{h,cpp}      # 加速モード → リレー / LED
test/
├── mock/
│   ├── Arduino.h                         # ネイティブテスト用スタブ
│   ├── MockCanGateway.h
│   └── MockGpioGateway.h
├── test_accel_mode/
└── test_speed_pulse/
```

> **wifi / dashboard について**: Phase 2 用に実装していたが、旧アーキ参照（`shared/IGateway.h` 等）で現行 IF と非互換のため削除済み。復活が必要な場合は git 履歴から参照し、`GatewaySet`/`ILifeCycle`/`ICanListener` に合わせて再実装すること。

### 加速モード → EDFC5 マッピング（`AccelMode::applyMode`）
| モード | リレー | LED |
|--------|--------|-----|
| Chill    | EXT2 ON（ソフト）   | 青 |
| Standard | 両 OFF（マニュアル値）| 青＋黄 |
| Sport    | EXT1 ON（ハード）   | 黄 |

### 開発環境
```
IDE        : VS Code + PlatformIO IDE 拡張機能
接続        : Windows PC → SSH → Mac mini（開発実機）
ビルド env  : rejsacan-esp32s3（board: esp32-s3-devkitc-1）
Arduino core: 2.0.17（LEDC は 2.x API: ledcSetup / ledcAttachPin / ledcChangeFrequency）
ビルドフラグ : -DARDUINO_USB_MODE=1 -DARDUINO_USB_CDC_ON_BOOT=1
単体テスト  : pio test -e native（AccelMode / SpeedPulse：native ランナー）
```

---

## 実装フェーズ

### Phase 1: CAN 確認 + 中核ロジック（現在）
- [x] アーキテクチャ実装（shared / gateway / feature）・ビルド成功
- [x] CanGateway: 全 CAN メッセージのシリアルダンプ実装（モード ID 探索用）
- [x] SpeedPulse / AccelMode 実装（リスナー登録・CAN 受信タスク）
- [x] 単体テスト（native）実装
- [ ] RejsaCAN ボード到着確認・ピン検証
- [ ] シリアルモニターで全 CAN メッセージダンプ → ID=0x257 で車速受信確認
- [ ] 加速モード変更時に変化する CAN ID を特定 → `CAN_MODE_ID` / `parseAccelMode()` 実装

### Phase 2: WebApp
- [ ] WifiGateway / Dashboard を現行 IF（GatewaySet / ILifeCycle / ICanListener）に合わせて再実装
- [ ] WiFi オンデマンド起動（ボタン GPIO）
- [ ] http://192.168.4.1 でダッシュボード確認（速度・モードのリアルタイム更新）

### Phase 3: EDFC5 連携（EDFC5 購入後）
- [ ] EDFC5 外部入力端子の電圧仕様確認・リレーモジュール配線
- [ ] `PULSES_PER_KM` を EDFC5 仕様に合わせて調整
- [ ] リレー極性（active-high / low）を実機確認
- [ ] 実車テスト

### Phase 4: 製品化（将来）
- [ ] OTA ファームウェアアップデート実装
- [ ] JLCPCB PCBA 発注
- [ ] 免責事項整備・販売準備

---

## 重要な決定事項・背景

### CAN アクセス
- Tesla 公式（CD-20-17-001）で Pin6=CANH / Pin14=CANL / 500kbps 確定
- DI_vehicleSpeed は CSS Electronics 実走・ScanMyTesla で実証済み
- 読み取り専用のため Tesla 保証リスクは最小限

### EDFC5 GPS キット代替
- TEIN GPS キット（¥13,200）は屋内・トンネルで信号ロスト問題あり
- CAN 直読みの DI_vehicleSpeed（精度 0.08kph）で完全代替可能

### 製品販売
- Tesla OTA で CAN ID が変わるリスク → OTA ファーム更新で対応予定
- 対応年式: Tesla Model 3 旧式（〜2023年9月）専用
- 販売時は免責事項（Tesla 保証・PL 法）の整備が必要

---

## TODO メモ（ハード到着後）

1. `CanGateway.h` の `CAN_MODE_ID = 0x000` → 実測 ID に変更、`parseAccelMode()` をビット仕様に合わせ実装
2. `GpioGateway.cpp` の `PULSES_PER_KM = 4000.0f` → EDFC5 マニュアルで確認
3. リレー / パルス用ピン（GPIO6,7,15）の確定・リレー極性確認
