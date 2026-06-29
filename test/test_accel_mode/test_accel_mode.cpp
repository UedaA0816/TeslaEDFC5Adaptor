// ============================================================
// AccelMode feature unit test
// ============================================================

#include <unity.h>

#include "MockCanGateway.h"
#include "MockGpioGateway.h"
#include "shared/GatewaySet.h"
#include "features/accel_mode/AccelMode.h"

// ── begin() が自身を CAN リスナーに登録すること ────────────────
void test_begin_registers_as_listener() {
    MockCanGateway can;
    MockGpioGateway gpio;
    GatewaySet gw = { can, gpio };
    AccelMode am(gw);

    am.begin();

    TEST_ASSERT_EQUAL(1, can.addListenerCount);
    // 多重継承のため ICanListener* と AccelMode* はアドレスが異なりうる
    TEST_ASSERT_EQUAL_PTR(static_cast<ICanListener*>(&am), can.lastListener);
}

// ── Chill → EXT2 ON, LED 青 ──────────────────────────────────
void test_chill_mode() {
    MockCanGateway can;
    MockGpioGateway gpio;
    GatewaySet gw = { can, gpio };
    AccelMode am(gw);

    am.onAccelModeUpdate(AccelModeType::CHILL);

    TEST_ASSERT_FALSE(gpio.ext1);
    TEST_ASSERT_TRUE(gpio.ext2);
    TEST_ASSERT_TRUE(gpio.blue);
    TEST_ASSERT_FALSE(gpio.yellow);
}

// ── Standard → 両 OFF, LED 青+黄 ────────────────────────────
void test_standard_mode() {
    MockCanGateway can;
    MockGpioGateway gpio;
    GatewaySet gw = { can, gpio };
    AccelMode am(gw);

    am.onAccelModeUpdate(AccelModeType::STANDARD);

    TEST_ASSERT_FALSE(gpio.ext1);
    TEST_ASSERT_FALSE(gpio.ext2);
    TEST_ASSERT_TRUE(gpio.blue);
    TEST_ASSERT_TRUE(gpio.yellow);
}

// ── Sport → EXT1 ON, LED 黄 ─────────────────────────────────
void test_sport_mode() {
    MockCanGateway can;
    MockGpioGateway gpio;
    GatewaySet gw = { can, gpio };
    AccelMode am(gw);

    am.onAccelModeUpdate(AccelModeType::SPORT);

    TEST_ASSERT_TRUE(gpio.ext1);
    TEST_ASSERT_FALSE(gpio.ext2);
    TEST_ASSERT_FALSE(gpio.blue);
    TEST_ASSERT_TRUE(gpio.yellow);
}

// ── Unknown → 全 OFF ────────────────────────────────────────
void test_unknown_mode() {
    MockCanGateway can;
    MockGpioGateway gpio;
    GatewaySet gw = { can, gpio };
    AccelMode am(gw);

    am.onAccelModeUpdate(AccelModeType::UNKNOWN);

    TEST_ASSERT_FALSE(gpio.ext1);
    TEST_ASSERT_FALSE(gpio.ext2);
    TEST_ASSERT_FALSE(gpio.blue);
    TEST_ASSERT_FALSE(gpio.yellow);
}

// ── モード切替で relay と LED が毎回呼ばれること ────────────────
void test_mode_change_calls_relay_and_led() {
    MockCanGateway can;
    MockGpioGateway gpio;
    GatewaySet gw = { can, gpio };
    AccelMode am(gw);

    am.onAccelModeUpdate(AccelModeType::CHILL);
    am.onAccelModeUpdate(AccelModeType::SPORT);

    TEST_ASSERT_EQUAL(2, gpio.relayCount);
    TEST_ASSERT_EQUAL(2, gpio.ledCount);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_begin_registers_as_listener);
    RUN_TEST(test_chill_mode);
    RUN_TEST(test_standard_mode);
    RUN_TEST(test_sport_mode);
    RUN_TEST(test_unknown_mode);
    RUN_TEST(test_mode_change_calls_relay_and_led);
    return UNITY_END();
}
