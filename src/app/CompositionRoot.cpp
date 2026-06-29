// ============================================================
// app/CompositionRoot.cpp
// ★ board 固有の薄い組立ファイル。
//   concrete 型を一切 include せず、ピン定義 + installer 呼び出し + App 生成のみ。
// ============================================================

#include <Arduino.h>

#include "app/App.h"
#include "app/CompositionRoot.h"
#include "DIContainer.h"

#include "gateway/GatewayInstaller.h"
#include "features/FeatureInstaller.h"

IApp& appInstance() {
    static DIContainer container = [] {
        DIContainer c;
        // board 固有ピン（RejsaCAN-ESP32-S3 v3.4）
        gateways::install(c, { GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_38 });
        features::install(c);
        return c;
    }();
    static App app(container.components());
    return app;
}
