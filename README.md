# ディレクトリ構成
```
src/
├── main.cpp                ← 初期化・依存注入のみ
├── shared/                 ← 型・インターフェース定義（依存なし）
│   ├── TeslaSignals.h
│   ├── ICanListener.h
│   ├── IGpioOutput.h
│   └── IWifiOutput.h
├── gateway/                ← I/O責任（ハード依存あり）
│   ├── can/CanGateway
│   ├── gpio/GpioGateway
│   └── wifi/WifiGateway
└── features/               ← ビジネスロジック（ハード依存ゼロ）
    ├── dashboard/Dashboard
    ├── speed_pulse/SpeedPulse
    └── accel_mode/AccelMode
```