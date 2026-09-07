/**
 * MeloTypes.h
 *
 * Common types, enums, and constants for the Melo SDK.
 *
 * SDK Version: 0.1.0
 */

#ifndef MELO_TYPES_H
#define MELO_TYPES_H

#include <Arduino.h>

// ---------------------------------------------------------------------------
// Motor / Direction Types
// ---------------------------------------------------------------------------

/** Motor selection */
enum MeloMotor {
    MOTOR_LEFT  = 0,
    MOTOR_RIGHT = 1,
    MOTOR_BOTH  = 2
};

/** Motor direction */
enum MeloDirection {
    DIR_FORWARD  = 0,
    DIR_BACKWARD = 1,
    DIR_STOP     = 2
};

/** Turn direction */
enum MeloTurnDir {
    TURN_LEFT  = 0,
    TURN_RIGHT = 1
};

// ---------------------------------------------------------------------------
// Status / Error Types
// ---------------------------------------------------------------------------

/** SDK initialization and operation status */
enum MeloStatus {
    MELO_OK                = 0,
    MELO_ERR_NOT_INIT      = 1,   // begin() not called
    MELO_ERR_DEVICE_NOT_FOUND = 2,
    MELO_ERR_TIMEOUT       = 3,
    MELO_ERR_FIRMWARE_MISMATCH = 4,
    MELO_ERR_PROTOCOL_MISMATCH = 5,
    MELO_ERR_INVALID_ARG   = 6,
    MELO_ERR_INVALID_CMD   = 7,
    MELO_ERR_SENSOR        = 8,
    MELO_ERR_CALIBRATION   = 9,
    MELO_ERR_MOTOR         = 10,
    MELO_ERR_TRANSPORT     = 11,
    MELO_ERR_UNKNOWN       = 99
};

// ---------------------------------------------------------------------------
// Device Info Structure
// ---------------------------------------------------------------------------

/** Holds device and version information returned by Melo.getDeviceInfo() */
struct MeloDeviceInfo {
    String hwVersion;
    String firmwareVersion;
    uint8_t protocolVersion;
    String sdkVersion;
    bool isCompatible;
};

// ---------------------------------------------------------------------------
// Sensor Reading Structures
// ---------------------------------------------------------------------------

/** Result of a distance sensor reading */
struct MeloDistanceReading {
    float distanceCm;    // Distance in centimeters; -1 if out of range or error
    bool valid;          // true if reading is within valid range
    MeloStatus status;   // MELO_OK or error code
};

/** Result of a line sensor reading */
struct MeloLineSensorReading {
    bool onLine;         // true = line detected (dark surface)
    uint16_t rawValue;   // Raw ADC or digital value
    bool valid;
    MeloStatus status;
};

// ---------------------------------------------------------------------------
// Helper macros
// ---------------------------------------------------------------------------

/** Clamp a value between min and max */
#define MELO_CLAMP(val, lo, hi)  ((val) < (lo) ? (lo) : ((val) > (hi) ? (hi) : (val)))

/** Convert percent (0-100) to 8-bit PWM (0-255) */
#define PCT_TO_PWM(pct)  ((uint8_t)(MELO_CLAMP((pct), 0, 100) * 255 / 100))

#endif // MELO_TYPES_H
