// ============================================================
// main.cpp
// 責務: Arduino の setup()/loop() を board 非依存の IApp に橋渡しするだけ。
//   配線（ピン・gateway 生成・DI 登録）は app/CompositionRoot.cpp に集約。
// ============================================================

#include "app/CompositionRoot.h"

void setup() { appInstance().setup(); }
void loop()  { appInstance().loop(); }
