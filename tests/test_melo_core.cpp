/**
 * test_melo_core.cpp
 *
 * Host evaluation and unit tests for Melo SDK core logic:
 *   - Protocol frame serialization and XOR checksum integrity
 *   - Clamping macros and percentage-to-PWM conversions
 *   - Status codes and protocol command definitions
 */

#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdint>

#include "../src/MeloTypes.h"
#include "../src/protocol/MeloProtocol.h"

int tests_passed = 0;
int tests_failed = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "  [FAIL] " << msg << " (line " << __LINE__ << ")" << std::endl; \
            tests_failed++; \
        } else { \
            std::cout << "  [PASS] " << msg << std::endl; \
            tests_passed++; \
        } \
    } while(0)

void test_clamping_and_math() {
    std::cout << "\n1. Running Clamping & Math Tests..." << std::endl;

    // Test clamp within range
    TEST_ASSERT(MELO_CLAMP(50, 0, 100) == 50, "Clamp within normal range [0, 100]");

    // Test clamp below min
    TEST_ASSERT(MELO_CLAMP(-25, 0, 100) == 0, "Clamp negative below min to 0");
    TEST_ASSERT(MELO_CLAMP(-150, -100, 100) == -100, "Clamp speed below -100 to -100");

    // Test clamp above max
    TEST_ASSERT(MELO_CLAMP(150, 0, 100) == 100, "Clamp value above 100 to 100");
    TEST_ASSERT(MELO_CLAMP(250, 0, 180) == 180, "Clamp servo angle above 180 to 180");

    // Test PWM conversions
    TEST_ASSERT(PCT_TO_PWM(0) == 0, "0% maps to 0 PWM");
    TEST_ASSERT(PCT_TO_PWM(100) == 255, "100% maps to 255 PWM");
    TEST_ASSERT(PCT_TO_PWM(50) == 127, "50% maps to 127 PWM");
    TEST_ASSERT(PCT_TO_PWM(150) == 255, "150% is clamped to 255 PWM");
}

void test_protocol_framing() {
    std::cout << "\n2. Running Protocol Contract & Checksum Tests..." << std::endl;

    // Build frame: CMD_MOTOR_MOVE_FORWARD (speed = 80)
    uint8_t payload[] = { 80 };
    uint8_t buffer[64];

    uint8_t frameLen = MeloProto::buildFrame(buffer, CMD_MOTOR_MOVE_FORWARD, payload, sizeof(payload));
    TEST_ASSERT(frameLen == 6, "Frame length for 1-byte payload is 6 bytes (START + CMD + LEN + PAYLOAD + CS + END)");
    TEST_ASSERT(buffer[0] == PROTO_FRAME_START, "Frame starts with PROTO_FRAME_START (0xAA)");
    TEST_ASSERT(buffer[1] == (uint8_t)CMD_MOTOR_MOVE_FORWARD, "Command byte matches CMD_MOTOR_MOVE_FORWARD");
    TEST_ASSERT(buffer[2] == 1, "Payload length is 1");
    TEST_ASSERT(buffer[3] == 80, "Payload content is 80");
    TEST_ASSERT(buffer[frameLen - 1] == PROTO_FRAME_END, "Frame ends with PROTO_FRAME_END (0x55)");

    // Verify XOR Checksum
    uint8_t expectedCs = buffer[1] ^ buffer[2] ^ buffer[3];
    TEST_ASSERT(buffer[4] == expectedCs, "Checksum matches XOR computation of CMD + LEN + PAYLOAD");

    // Validate checksum helper
    uint8_t computedCs = MeloProto::checksum(&buffer[1], 3);
    TEST_ASSERT(computedCs == buffer[4], "MeloProto::checksum correctly verifies frame checksum");

    // Corrupt payload and verify checksum mismatch
    buffer[3] ^= 0xFF; // Flip payload bits
    uint8_t corruptedCs = MeloProto::checksum(&buffer[1], 3);
    TEST_ASSERT(corruptedCs != buffer[4], "Checksum detects corrupted payload byte");

    // Build frame: CMD_BUZZER_BEEP (freq: 1000Hz = 0x03E8, duration: 250ms = 0xFA)
    uint8_t beepPayload[] = { 0x03, 0xE8, 0x00, 0xFA };
    frameLen = MeloProto::buildFrame(buffer, CMD_BUZZER_BEEP, beepPayload, sizeof(beepPayload));
    TEST_ASSERT(frameLen == 9, "Frame length for 4-byte payload is 9 bytes");
    TEST_ASSERT(buffer[1] == (uint8_t)CMD_BUZZER_BEEP, "Command byte matches CMD_BUZZER_BEEP");
    TEST_ASSERT(buffer[2] == 4, "Payload length is 4");
    TEST_ASSERT(MeloProto::checksum(&buffer[1], 6) == buffer[7], "Multi-byte payload checksum verified");
}

void test_response_codes() {
    std::cout << "\n3. Running Response Codes & Error Definitions..." << std::endl;

    TEST_ASSERT(RESP_OK == 0x00, "RESP_OK is 0x00");
    TEST_ASSERT(RESP_ERR_INVALID_CMD == 0x02, "RESP_ERR_INVALID_CMD is 0x02");
    TEST_ASSERT(RESP_ERR_INVALID_ARG == 0x03, "RESP_ERR_INVALID_ARG is 0x03");
    TEST_ASSERT(RESP_ERR_SENSOR == 0x04, "RESP_ERR_SENSOR is 0x04");
    TEST_ASSERT(RESP_ERR_TIMEOUT == 0x06, "RESP_ERR_TIMEOUT is 0x06");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Melo SDK Core Evaluation Test Suite   " << std::endl;
    std::cout << "========================================" << std::endl;

    test_clamping_and_math();
    test_protocol_framing();
    test_response_codes();

    std::cout << "\n========================================" << std::endl;
    std::cout << "Test Results: " << tests_passed << " Passed, " << tests_failed << " Failed" << std::endl;
    std::cout << "========================================" << std::endl;

    return (tests_failed == 0) ? 0 : 1;
}
