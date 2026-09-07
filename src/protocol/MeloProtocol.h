/**
 * MeloProtocol.h
 *
 * Melo SDK ↔ ESP Firmware Protocol Contract
 *
 * This file defines the formal communication contract between the
 * Melo SDK and ESP firmware. It establishes:
 *   - Command identifiers
 *   - Argument types and valid ranges
 *   - Response format
 *   - Error codes
 *   - Protocol versioning
 *
 * NOTE: In SDK Model A (direct Arduino on ESP), this protocol layer
 * is used internally by the SDK for future extensibility and to define
 * a stable capability contract for Melo Blocks / Web.
 *
 * In SDK Model B (host ↔ firmware command protocol), this file becomes
 * the wire protocol specification.
 *
 * Protocol Version: 1
 * SDK Version: 0.1.0
 */

#ifndef MELO_PROTOCOL_H
#define MELO_PROTOCOL_H

#include <Arduino.h>

// ---------------------------------------------------------------------------
// Protocol Version
// ---------------------------------------------------------------------------
#define MELO_PROTOCOL_VERSION_CURRENT  1

// ---------------------------------------------------------------------------
// Command Identifiers
// Each command has a unique 8-bit ID.
// ---------------------------------------------------------------------------
enum MeloCommand : uint8_t {
    // System commands
    CMD_PING                = 0x01,   // Check if device is alive
    CMD_GET_DEVICE_INFO     = 0x02,   // Request device info
    CMD_GET_FW_VERSION      = 0x03,   // Request firmware version
    CMD_GET_PROTOCOL_VER    = 0x04,   // Request protocol version
    CMD_RESET               = 0x05,   // Soft reset device

    // Motor commands
    CMD_MOTOR_SET_SPEED     = 0x10,   // Set single motor speed
    CMD_MOTOR_MOVE_FORWARD  = 0x11,   // Drive forward
    CMD_MOTOR_MOVE_BACKWARD = 0x12,   // Drive backward
    CMD_MOTOR_TURN_LEFT     = 0x13,   // Turn left
    CMD_MOTOR_TURN_RIGHT    = 0x14,   // Turn right
    CMD_MOTOR_STOP          = 0x15,   // Stop all motors
    CMD_MOTOR_COAST         = 0x16,   // Coast all motors

    // Servo commands
    CMD_SERVO_SET_ANGLE     = 0x20,   // Set servo angle

    // Sensor commands
    CMD_SENSOR_READ_DISTANCE = 0x30,  // Read ultrasonic distance
    CMD_SENSOR_READ_LINE    = 0x31,   // Read line sensor
    CMD_SENSOR_READ_LIGHT   = 0x32,   // Read light sensor

    // LED commands
    CMD_LED_SET             = 0x40,   // Set LED on/off

    // Buzzer commands
    CMD_BUZZER_BEEP         = 0x50,   // Play tone for duration
    CMD_BUZZER_START        = 0x51,   // Start continuous tone
    CMD_BUZZER_STOP         = 0x52,   // Stop tone

    // Calibration commands
    CMD_CALIBRATE           = 0x60,   // Run calibration
    CMD_CALIBRATE_MOTORS    = 0x61,   // Calibrate motors
    CMD_CALIBRATE_SENSORS   = 0x62,   // Calibrate sensors
};

// ---------------------------------------------------------------------------
// Response Codes
// ---------------------------------------------------------------------------
enum MeloResponseCode : uint8_t {
    RESP_OK             = 0x00,   // Command succeeded
    RESP_ERR_UNKNOWN    = 0x01,   // Unknown error
    RESP_ERR_INVALID_CMD  = 0x02, // Command ID not recognized
    RESP_ERR_INVALID_ARG  = 0x03, // Argument out of valid range
    RESP_ERR_SENSOR     = 0x04,   // Sensor failure
    RESP_ERR_MOTOR      = 0x05,   // Motor failure
    RESP_ERR_TIMEOUT    = 0x06,   // Operation timed out
    RESP_ERR_NOT_INIT   = 0x07,   // Device not initialized
    RESP_ERR_CAL_FAIL   = 0x08,   // Calibration failure
};

// ---------------------------------------------------------------------------
// Command Argument Specifications
//
// All ranges represent valid inputs that the firmware must accept.
// Out-of-range arguments must return RESP_ERR_INVALID_ARG.
// ---------------------------------------------------------------------------

// CMD_MOTOR_SET_SPEED
//   arg0: uint8_t  motor (0=LEFT, 1=RIGHT, 2=BOTH)
//   arg1: int8_t   speed (-100 to 100; positive=forward, negative=backward)
#define MOTOR_SPEED_MIN   -100
#define MOTOR_SPEED_MAX    100

// CMD_MOTOR_MOVE_FORWARD / CMD_MOTOR_MOVE_BACKWARD
//   arg0: uint8_t  speed (0 to 100)
#define MOTOR_SPEED_PCT_MIN  0
#define MOTOR_SPEED_PCT_MAX  100

// CMD_MOTOR_TURN_LEFT / CMD_MOTOR_TURN_RIGHT
//   arg0: uint8_t  speed (0 to 100)

// CMD_SERVO_SET_ANGLE
//   arg0: uint8_t  servo_id (1 to SERVO_COUNT)
//   arg1: uint8_t  angle (0 to 180 degrees)
#define SERVO_ANGLE_MIN   0
#define SERVO_ANGLE_MAX   180

// CMD_SENSOR_READ_DISTANCE
//   No arguments
//   Response: float32  distance_cm (-1.0 if error/out-of-range)

// CMD_SENSOR_READ_LINE
//   No arguments
//   Response: uint8_t  on_line (0=no line, 1=line detected)
//             uint16_t raw_value

// CMD_LED_SET
//   arg0: uint8_t  led_id (1 to LED_COUNT)
//   arg1: uint8_t  state (0=off, 1=on)

// CMD_BUZZER_BEEP
//   arg0: uint16_t frequency_hz (0 to 20000; 0=silence)
//   arg1: uint32_t duration_ms
#define BUZZER_FREQ_MIN    0
#define BUZZER_FREQ_MAX    20000

// ---------------------------------------------------------------------------
// Packet Frame Format (for Model B / future protocol use)
//
// | START | CMD | LEN | PAYLOAD[LEN] | CHECKSUM | END |
// |  0xAA | 1B  | 1B  |    N bytes   |    1B    | 0x55|
//
// CHECKSUM = XOR of CMD + LEN + all PAYLOAD bytes
// ---------------------------------------------------------------------------
#define PROTO_FRAME_START  0xAA
#define PROTO_FRAME_END    0x55
#define PROTO_MAX_PAYLOAD  32   // Maximum payload bytes per packet

// ---------------------------------------------------------------------------
// Protocol Utility Functions (host-side encoding helpers)
// ---------------------------------------------------------------------------

namespace MeloProto {

    /** Calculate XOR checksum of a byte buffer */
    inline uint8_t checksum(const uint8_t* data, uint8_t len) {
        uint8_t cs = 0;
        for (uint8_t i = 0; i < len; i++) cs ^= data[i];
        return cs;
    }

    /** Build a minimal command frame into buffer.
     *  Returns total frame length in bytes.
     *  buffer must be at least (5 + payloadLen) bytes. */
    inline uint8_t buildFrame(uint8_t* buffer, MeloCommand cmd,
                               const uint8_t* payload, uint8_t payloadLen) {
        buffer[0] = PROTO_FRAME_START;
        buffer[1] = (uint8_t)cmd;
        buffer[2] = payloadLen;
        for (uint8_t i = 0; i < payloadLen; i++) {
            buffer[3 + i] = payload[i];
        }
        uint8_t cs = MeloProto::checksum(&buffer[1], 2 + payloadLen);
        buffer[3 + payloadLen] = cs;
        buffer[4 + payloadLen] = PROTO_FRAME_END;
        return 5 + payloadLen;
    }

}  // namespace MeloProto

#endif // MELO_PROTOCOL_H
