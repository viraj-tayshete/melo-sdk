/**
 * test_melo_core.cpp
 *
 * Host unit tests for the hardware-independent parts of the Melo SDK:
 *   - clamp macro
 *   - wheel speed -> servo pulse -> PWM duty maths
 *   - single-sensor line following state machine
 *
 * Run with tests/run.sh
 */

#include <iostream>
#include <cstdint>

#include "../src/MeloTypes.h"
#include "../src/drive/WheelMath.h"
#include "../src/behaviors/LineFollowLogic.h"
#include "../src/ports/PortMath.h"
#include "../src/PinRules.h"

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "  [FAIL] " << msg << " (line " << __LINE__ << ")" << std::endl; \
            tests_failed++; \
        } else { \
            std::cout << "  [PASS] " << msg << std::endl; \
            tests_passed++; \
        } \
    } while (0)

static void test_clamp() {
    std::cout << "\n1. Clamp macro" << std::endl;
    TEST_ASSERT(MELO_CLAMP(50, 0, 100) == 50, "value inside range is unchanged");
    TEST_ASSERT(MELO_CLAMP(-25, 0, 100) == 0, "value below range goes to lo");
    TEST_ASSERT(MELO_CLAMP(250, 0, 100) == 100, "value above range goes to hi");
}

static void test_wheel_pulse() {
    std::cout << "\n2. Wheel pulse maths" << std::endl;
    TEST_ASSERT(meloWheelPulseUs(0, false, 0) == 1500, "speed 0 is the centre pulse");
    TEST_ASSERT(meloWheelPulseUs(100, false, 0) == 2000, "full forward, normal servo = 2000us");
    TEST_ASSERT(meloWheelPulseUs(-100, false, 0) == 1000, "full backward, normal servo = 1000us");
    TEST_ASSERT(meloWheelPulseUs(100, true, 0) == 1000, "full forward, mirrored servo = 1000us");
    TEST_ASSERT(meloWheelPulseUs(-100, true, 0) == 2000, "full backward, mirrored servo = 2000us");
    TEST_ASSERT(meloWheelPulseUs(50, false, 0) == 1750, "half speed = 1750us");
    TEST_ASSERT(meloWheelPulseUs(0, false, 20) == 1520, "trim shifts the centre");
    TEST_ASSERT(meloWheelPulseUs(0, true, 20) == 1520, "trim is not mirrored with the direction");
    TEST_ASSERT(meloWheelPulseUs(100, false, 200) == MELO_SERVO_MAX_US, "pulse is clamped to the maximum");
    TEST_ASSERT(meloWheelPulseUs(-100, false, -200) == MELO_SERVO_MIN_US, "pulse is clamped to the minimum");
    TEST_ASSERT(meloWheelPulseUs(127, false, 0) == 2000, "speed above 100 is treated as 100");
    TEST_ASSERT(meloWheelPulseUs(-128, false, 0) == 1000, "speed below -100 is treated as -100");

    bool symmetric = true;
    for (int s = 1; s <= 100; s++) {
        int up   = (int)meloWheelPulseUs(s, false, 0) - 1500;
        int down = 1500 - (int)meloWheelPulseUs(-s, false, 0);
        if (up != down) symmetric = false;
    }
    TEST_ASSERT(symmetric, "forward and backward speeds are symmetric around the centre");

    TEST_ASSERT(meloPulseToDuty(1500) == 1228, "1500us -> duty 1228 (14-bit, 50Hz)");
    TEST_ASSERT(meloPulseToDuty(2000) == 1638, "2000us -> duty 1638");
    TEST_ASSERT(meloPulseToDuty(1000) == 819,  "1000us -> duty 819");
}

static void test_line_follow_basic() {
    std::cout << "\n3. Line following: steering" << std::endl;
    MeloLineFollowLogic f;

    MeloDriveCmd on = f.step(true, 1000, 50);
    TEST_ASSERT(on.right == 50 && on.left == 7, "on the line: arc left (left wheel slow)");
    TEST_ASSERT(f.state() == LINE_FOLLOWING, "state is FOLLOWING on the line");

    MeloDriveCmd off = f.step(false, 1010, 50);
    TEST_ASSERT(off.left == 50 && off.right == 7, "off the line: arc right (right wheel slow)");
    TEST_ASSERT(f.state() == LINE_FOLLOWING, "a short gap does not trigger the search");

    MeloLineFollowLogic g;
    MeloDriveCmd fast = g.step(true, 0, 250);
    TEST_ASSERT(fast.right == 100, "speed above 100 is clamped");
}

static void test_line_follow_search() {
    std::cout << "\n4. Line following: lost line and search" << std::endl;
    MeloLineFollowLogic f;
    f.step(true, 1000, 50);

    f.step(false, 1000 + MELO_LINE_LOST_MS - 1, 50);
    TEST_ASSERT(f.state() == LINE_FOLLOWING, "still FOLLOWING just before the lost timeout");

    uint32_t t = 1000 + MELO_LINE_LOST_MS;
    MeloDriveCmd c = f.step(false, t, 50);
    TEST_ASSERT(f.state() == LINE_SEARCHING, "SEARCHING once the line has been gone for the lost timeout");
    TEST_ASSERT(c.left == -MELO_LINE_SEARCH_SPEED_PCT && c.right == MELO_LINE_SEARCH_SPEED_PCT,
                "first sweep pivots left on the spot");

    c = f.step(false, t + MELO_LINE_SEARCH_STEP_MS, 50);
    TEST_ASSERT(c.left == MELO_LINE_SEARCH_SPEED_PCT && c.right == -MELO_LINE_SEARCH_SPEED_PCT,
                "second sweep pivots right");

    c = f.step(false, t + MELO_LINE_SEARCH_STEP_MS + 2 * MELO_LINE_SEARCH_STEP_MS, 50);
    TEST_ASSERT(c.left == -MELO_LINE_SEARCH_SPEED_PCT, "third sweep pivots left again, each sweep longer than the last");

    c = f.step(true, t + 2000, 50);
    TEST_ASSERT(f.state() == LINE_FOLLOWING, "finding the line returns to FOLLOWING");
    TEST_ASSERT(c.right == 50, "and steers normally in the same step");

    MeloLineFollowLogic slow;
    slow.step(true, 0, 20);
    slow.step(false, MELO_LINE_LOST_MS, 20);
    MeloDriveCmd p = slow.step(false, MELO_LINE_LOST_MS + 1, 20);
    TEST_ASSERT(p.right == 20, "search speed never exceeds the requested speed");
}

static void test_line_follow_give_up() {
    std::cout << "\n5. Line following: giving up" << std::endl;
    MeloLineFollowLogic f;
    f.step(true, 0, 50);
    uint32_t t = MELO_LINE_LOST_MS;
    f.step(false, t, 50);
    TEST_ASSERT(f.state() == LINE_SEARCHING, "search started");

    MeloDriveCmd c = f.step(false, t + MELO_LINE_SEARCH_MAX_MS - 1, 50);
    TEST_ASSERT(f.state() == LINE_SEARCHING, "still searching just before the limit");

    c = f.step(false, t + MELO_LINE_SEARCH_MAX_MS, 50);
    TEST_ASSERT(f.state() == LINE_GAVE_UP, "gives up after the search limit");
    TEST_ASSERT(c.left == 0 && c.right == 0, "wheels are told to stop");

    c = f.step(true, t + MELO_LINE_SEARCH_MAX_MS + 10, 50);
    TEST_ASSERT(f.state() == LINE_GAVE_UP && c.left == 0 && c.right == 0, "stays given-up until reset");

    f.reset();
    c = f.step(true, 0, 50);
    TEST_ASSERT(f.state() == LINE_FOLLOWING && c.right == 50, "reset() starts a fresh run");
}

static void test_line_follow_time_wrap() {
    std::cout << "\n6. Line following: millis() wrap-around" << std::endl;
    MeloLineFollowLogic f;
    uint32_t start = 0xFFFFFFFFu - 500;
    f.step(true, start, 50);
    f.step(false, start + 100, 50);
    TEST_ASSERT(f.state() == LINE_FOLLOWING, "no false search across the wrap point");
    f.step(false, start + MELO_LINE_LOST_MS, 50);
    TEST_ASSERT(f.state() == LINE_SEARCHING, "lost timeout still works across the wrap point");
}

static void test_port_ohms() {
    std::cout << "\n7. Port ID resistor estimate" << std::endl;
    TEST_ASSERT(meloPortOhms(MELO_ADC_MAX) == -1, "full-scale reading = nothing plugged in");
    TEST_ASSERT(meloPortOhms(MELO_PORT_OPEN_RAW) == -1, "reading at the open threshold = nothing plugged in");
    TEST_ASSERT(meloPortOhms(0) == 0, "0 reading = shorted to ground");
    // 10k pull-up with a 10k block: the midpoint, raw 2048 -> about 10k
    int32_t mid = meloPortOhms(2048);
    TEST_ASSERT(mid >= 9900 && mid <= 10100, "midpoint reading estimates the pull-up value");
    // 4k7 block: raw = 4095 * 4700 / 14700 = 1309
    int32_t r47 = meloPortOhms(1309);
    TEST_ASSERT(r47 >= 4600 && r47 <= 4800, "a 4k7 block is estimated close to 4.7k");
    // 33k block: raw = 4095 * 33000 / 43000 = 3142
    int32_t r33 = meloPortOhms(3142);
    TEST_ASSERT(r33 >= 32000 && r33 <= 34000, "a 33k block is estimated close to 33k");
    bool monotonic = true;
    for (int raw = 1; raw < MELO_PORT_OPEN_RAW; raw++)
        if (meloPortOhms((uint16_t)raw) < meloPortOhms((uint16_t)(raw - 1))) monotonic = false;
    TEST_ASSERT(monotonic, "estimated resistance never falls as the reading rises");
}

static void test_pin_rules() {
    std::cout << "\n8. ESP32-S3-WROOM pin rules" << std::endl;
    int good[] = {1, 2, 4, 5, 10, 18, 21, 38, 42, 47, 48};
    bool allGood = true;
    for (int p : good) if (!meloGpioUsable(p)) allGood = false;
    TEST_ASSERT(allGood, "ordinary GPIOs (1,2,4..18,21,38..42,47,48) are usable");
    TEST_ASSERT(!meloGpioUsable(0) && !meloGpioUsable(3) && !meloGpioUsable(45) && !meloGpioUsable(46), "strapping pins are rejected");
    TEST_ASSERT(!meloGpioUsable(19) && !meloGpioUsable(20), "USB pins are rejected");
    TEST_ASSERT(!meloGpioUsable(22) && !meloGpioUsable(25), "GPIO22-25 (nonexistent) are rejected");
    TEST_ASSERT(!meloGpioUsable(26) && !meloGpioUsable(37), "flash/PSRAM pins 26-37 are rejected");
    TEST_ASSERT(!meloGpioUsable(43) && !meloGpioUsable(44), "UART0 pins are rejected");
    TEST_ASSERT(!meloGpioUsable(-1) && !meloGpioUsable(49), "out-of-range pins are rejected");
    bool defaultsOk = true;
    int defaults[] = {MELO_DEFAULT_PORT1_SIGNAL_PIN, MELO_DEFAULT_PORT2_SIGNAL_PIN, MELO_DEFAULT_PORT3_SIGNAL_PIN,
                      MELO_DEFAULT_PORT4_SIGNAL_PIN, MELO_DEFAULT_PORT5_SIGNAL_PIN, MELO_DEFAULT_PORT6_SIGNAL_PIN,
                      MELO_DEFAULT_PORT1_ADD_PIN, MELO_DEFAULT_PORT2_ADD_PIN, MELO_DEFAULT_PORT3_ADD_PIN,
                      MELO_DEFAULT_PORT4_ADD_PIN, MELO_DEFAULT_PORT5_ADD_PIN, MELO_DEFAULT_PORT6_ADD_PIN,
                      MELO_DEFAULT_LEFT_WHEEL_PIN, MELO_DEFAULT_RIGHT_WHEEL_PIN};
    for (int p : defaults) if (!meloGpioUsable(p)) defaultsOk = false;
    TEST_ASSERT(defaultsOk, "every default pin passes the rules");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Melo SDK Host Test Suite             " << std::endl;
    std::cout << "========================================" << std::endl;

    test_clamp();
    test_wheel_pulse();
    test_line_follow_basic();
    test_line_follow_search();
    test_line_follow_give_up();
    test_line_follow_time_wrap();
    test_port_ohms();
    test_pin_rules();

    std::cout << "\n========================================" << std::endl;
    std::cout << "Results: " << tests_passed << " passed, " << tests_failed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;
    return tests_failed == 0 ? 0 : 1;
}
