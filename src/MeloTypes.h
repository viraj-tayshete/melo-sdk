/**
 * MeloTypes.h
 *
 * Common types and enums for the Melo SDK.
 *
 * SDK Version: 0.2.0
 */

#ifndef MELO_TYPES_H
#define MELO_TYPES_H

#ifdef ARDUINO
  #include <Arduino.h>
#else
  #include <cstdint>
  #include <cstddef>
  #include <string>
  typedef std::string String;
#endif

/** Which wheel a command applies to. */
enum MeloWheelSide {
    WHEEL_LEFT  = 0,
    WHEEL_RIGHT = 1,
    WHEEL_BOTH  = 2
};

/**
 * Result codes. Keep in step with errorCodes in docs/CAPABILITY_MODEL.json
 * (tools/check_capabilities.py enforces this).
 */
enum MeloStatus {
    MELO_OK             = 0,
    MELO_ERR_NOT_INIT   = 1,   // begin() not called, or it failed
    MELO_ERR_INVALID_ARG = 2,  // argument out of range
    MELO_ERR_PIN        = 3,   // a pin could not be set up (wrong or clashing pin)
    MELO_ERR_LINE_LOST  = 4    // followLine() could not find the line again
};

/** Information about this robot, returned by Melo.getDeviceInfo(). */
struct MeloDeviceInfo {
    String sdkVersion;
    String hwVersion;
    String board;
};

/** Clamp a value between lo and hi. Arguments are evaluated more than once. */
#define MELO_CLAMP(val, lo, hi)  ((val) < (lo) ? (lo) : ((val) > (hi) ? (hi) : (val)))

#endif // MELO_TYPES_H
