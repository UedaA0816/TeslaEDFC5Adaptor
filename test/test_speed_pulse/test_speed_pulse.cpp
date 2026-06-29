// ============================================================
// SpeedPulse feature unit test
// ============================================================

#include <unity.h>

#include "MockCanGateway.h"
#include "MockGpioGateway.h"
#include "shared/GatewaySet.h"
#include "features/speed_pulse/SpeedPulse.h"

// ── begin() が自身を CAN リスナーに登録すること ────────────────
void test_begin_registers_as_listener() {
    MockCanGateway can;
    MockGpioGateway gpio;
    GatewaySet gw = { can, gpio };
    SpeedPulse sp(gw);

    sp.begin();

    TEST_ASSERT_EQUAL(1, can.addListenerCount);
    // 多重継承のため ICanListener* と SpeedPulse* はアドレスが異なりうる
    TEST_ASSERT_EQUAL_PTR(static_cast<ICanListener*>(&sp), can.lastListener);
}

// ── onSpeedUpdate → setSpeedPulse に正しく転送すること ─────────
void test_speed_update_forwards_to_gpio() {
    MockCanGateway can;
    MockGpioGateway gpio;
    GatewaySet gw = { can, gpio };
    SpeedPulse sp(gw);

    sp.onSpeedUpdate(60.0f);

    TEST_ASSERT_EQUAL(1, gpio.speedCount);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 60.0f, gpio.lastSpeed);
}

// ── 速度 0 でも正しく転送すること ────────────────────────────
void test_speed_zero_forwards() {
    MockCanGateway can;
    MockGpioGateway gpio;
    GatewaySet gw = { can, gpio };
    SpeedPulse sp(gw);

    sp.onSpeedUpdate(0.0f);

    TEST_ASSERT_EQUAL(1, gpio.speedCount);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, gpio.lastSpeed);
}

// ── リレー/LED に影響しないこと ──────────────────────────────
void test_speed_update_does_not_touch_relay_or_led() {
    MockCanGateway can;
    MockGpioGateway gpio;
    GatewaySet gw = { can, gpio };
    SpeedPulse sp(gw);

    sp.onSpeedUpdate(100.0f);

    TEST_ASSERT_EQUAL(0, gpio.relayCount);
    TEST_ASSERT_EQUAL(0, gpio.ledCount);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_begin_registers_as_listener);
    RUN_TEST(test_speed_update_forwards_to_gpio);
    RUN_TEST(test_speed_zero_forwards);
    RUN_TEST(test_speed_update_does_not_touch_relay_or_led);
    return UNITY_END();
}
