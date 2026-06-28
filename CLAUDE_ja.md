# Tesla CAN × TEIN EDFC5 連携システム（日本語リファレンス）

> このファイルは人間向けの詳細リファレンス。Claude が毎回読むのは英語版 `CLAUDE.md`。
> トークン節約のため、AI 向けの指示は `CLAUDE.md`（簡潔・英語）に集約し、
> 背景や経緯などの冗長な説明はこの `CLAUDE_ja.md` に置く。

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
GPIO 16  : WIFI_BUTTON（WiFi オンデマンド起動・Phase 2）← 暫定
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
- **Gateway**: I/O 責任（CAN 受信・GPIO 出力・WiFi）
- **Feature**: ビジネスロジックのみ（ハード依存ゼロ）
- **Shared**: 型・インターフェース定義
- **依存の方向**: Feature → Gateway（DI／インターフェース経由・逆方向なし）
- **Feature 間の依存**: なし
- **CAN 通知**: Feature が `ICanListener` を実装し `CanGateway::addListener()` で登録、コールバックを受ける

### ディレクトリ構成（実装済み）
```
src/
├── main.cpp                          # 初期化・DI 登録・gateway begin/update 駆動
├── DIContainer.h                     # 簡易 DI コンテナ
├── shared/
│   ├── TeslaSignals.h                # AccelModeType・accelModeStr()・TeslaSignals
│   ├── ICanListener.h                # CAN 通知 IF（デフォルト空実装の virtual）
│   ├── ICanGateway.h                 # begin / addListener / update
│   ├── IGpioGateway.h                # begin / setSpeedPulse / setEdfc5Relay / setLed
│   └── IFeature.h                    # setup / loop + diContainer()
├── gateway/
│   ├── can/CanGateway.{h,cpp}        # TWAI 受信・パース・リスナー通知
│   ├── gpio/GpioGateway.{h,cpp}      # LED・リレー・PWM 制御
│   └── wifi/WifiGateway.{h,cpp}      # ★ビルド除外中（Phase 2）
└── features/
    ├── speed_pulse/SpeedPulse.{h,cpp}   # 車速 → PWM パルス
    ├── accel_mode/AccelMode.{h,cpp}     # 加速モード → リレー / LED
    └── dashboard/Dashboard.{h,cpp}      # ★ビルド除外中（Phase 2）
```

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
ビルド      : pio run -e rejsacan-esp32s3 → ✅成功（RAM 5.8% / Flash 8.7%）
```

### wifi / dashboard の扱い
- Phase 2（WebApp）用にソースは温存しつつ、現在は `platformio.ini` の `build_src_filter` でビルド除外
  （`-<gateway/wifi/>` `-<features/dashboard/>`）。
- `lib_deps`（ESP Async WebServer）は**明示するとビルド対象になる**ため現在コメントアウト。
  Phase 2 で wifi/dashboard を有効化する際に復活させる（コメント参照）。

---

## 実装フェーズ

### Phase 1: Lチカ検証 + 中核ロジック（現在）
- [x] アーキテクチャ実装（shared / gateway / feature）・ビルド成功
- [x] CanGateway: 全 CAN メッセージのシリアルダンプ実装（モード ID 探索用）
- [x] SpeedPulse / AccelMode 実装（リスナー登録・CAN ポーリング）
- [ ] RejsaCAN ボード到着確認・ピン検証
- [ ] シリアルモニターで全 CAN メッセージダンプ → ID=0x257 で車速受信確認
- [ ] 加速モード変更時に変化する CAN ID を特定 → `CAN_MODE_ID` / `parseAccelMode()` 実装

### Phase 2: WebApp
- [ ] wifi / dashboard をビルド対象へ復帰（`build_src_filter` / `lib_deps` 戻し）
- [ ] WiFi オンデマンド起動（GPIO16 ボタン）
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

### WiFi 設計
- 常時 AP は電波セキュリティ上不安なためオンデマンド方式を採用
- 車両スリープ時は OBD2 Pin16 が電源断 → 自動シャットダウン
- SSID hidden + パスワード保護

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
4. Phase 2 で `WifiGateway.cpp` の `PASSWORD` を本番用に変更
