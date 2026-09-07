/**
 * Melo.h
 *
 * Melo SDK — Public API
 *
 * This is the single header users include in their Arduino sketches.
 * It provides the complete robot-level and component-level API for the
 * Melo robotics platform.
 *
 * Usage:
 *   #include <Melo.h>
 *
 *   void setup() {
 *       Melo.begin();
 *   }
 *
 *   void loop() {
 *       Melo.moveForward(70);
 *       delay(1000);
 *       Melo.stop();
 *       delay(1000);
 *   }
 *
 * SDK Version: 0.1.0
 * Target:      ESP32 (assumed) / ESP8266 compatible
 * License:     MIT
 *
 * IMPORTANT: Pin assignments are defined in src/MeloConfig.h.
 * All default values are PLACEHOLDERS — verify against the physical
 * Melo PCB schematic before deploying on real hardware.
 */

#ifndef MELO_H
#define MELO_H

#include <Arduino.h>
#include "MeloConfig.h"
#include "MeloTypes.h"
#include "motors/Motor.h"
#include "sensors/DistanceSensor.h"
#include "sensors/LineSensor.h"
#include "servos/MeloServo.h"
#include "leds/MeloLED.h"
#include "buzzer/MeloBuzzer.h"

// ---------------------------------------------------------------------------
// MeloClass — Main SDK Class
//
// Instantiated as a global singleton: extern MeloClass Melo;
// Users interact only with the global Melo object, not this class directly.
// ---------------------------------------------------------------------------

class MeloClass {
public:
    MeloClass();

    // -----------------------------------------------------------------------
    // SYSTEM / INITIALIZATION
    // -----------------------------------------------------------------------

    /**
     * Initialize the Melo robot.
     *
     * Must be called once in setup() before using any other Melo APIs.
     *
     * Performs:
     *   1. Initialize serial (for debug output)
     *   2. Initialize all hardware subsystems
     *   3. Move all actuators to safe starting positions
     *   4. Play startup tone
     *   5. Report status via serial
     *
     * @return  MELO_OK on success, error code on failure
     */
    MeloStatus begin();

    /**
     * Returns true if Melo.begin() was called successfully.
     */
    bool isReady() const { return _initialized; }

    /**
     * Get the SDK version string.
     * @return  Version string e.g. "0.1.0"
     */
    String getSdkVersion() const;

    /**
     * Get the firmware version string.
     * For SDK Model A (direct Arduino), this is the same as SDK version.
     * @return  Version string
     */
    String getFirmwareVersion() const;

    /**
     * Get device information (versions, compatibility).
     * @return  MeloDeviceInfo struct
     */
    MeloDeviceInfo getDeviceInfo() const;

    /**
     * Get the last error status.
     * @return  Last MeloStatus error code
     */
    MeloStatus lastError() const { return _lastError; }

    /**
     * Get a human-readable description of the last error.
     * @return  Human-readable error message string
     */
    String lastErrorMessage() const;

    /**
     * Get a human-readable description of any MeloStatus code.
     * @return  Human-readable description
     */
    String statusMessage(MeloStatus status) const;

    /**
     * Print device info and status to Serial.
     * Useful for diagnostics.
     */
    void printStatus() const;

    // -----------------------------------------------------------------------
    // ROBOT-LEVEL MOVEMENT APIs (High-level)
    // -----------------------------------------------------------------------

    /**
     * Move the robot forward at a given speed.
     *
     * @param speed  Speed percentage 0-100. Default: 70.
     *               0 = stopped, 100 = maximum speed.
     */
    void moveForward(uint8_t speed = 70);

    /**
     * Move the robot backward at a given speed.
     *
     * @param speed  Speed percentage 0-100. Default: 70.
     */
    void moveBackward(uint8_t speed = 70);

    /**
     * Turn the robot left (counter-clockwise) at a given speed.
     * Uses differential drive: right motor forward, left motor stopped/backward.
     *
     * @param speed  Turn speed percentage 0-100. Default: 60.
     */
    void turnLeft(uint8_t speed = TURN_DEFAULT_SPEED_PCT);

    /**
     * Turn the robot right (clockwise) at a given speed.
     * Uses differential drive: left motor forward, right motor stopped/backward.
     *
     * @param speed  Turn speed percentage 0-100. Default: 60.
     */
    void turnRight(uint8_t speed = TURN_DEFAULT_SPEED_PCT);

    /**
     * Stop all motors.
     * Actively brakes motors (holds position).
     */
    void stop();

    /**
     * Coast all motors to a stop (no active braking).
     * Motors spin down freely.
     */
    void coast();

    // -----------------------------------------------------------------------
    // COMPONENT-LEVEL MOTOR APIs
    // -----------------------------------------------------------------------

    /**
     * Set individual motor speed and direction.
     *
     * @param motor  MOTOR_LEFT, MOTOR_RIGHT, or MOTOR_BOTH
     * @param speed  Speed percentage -100 to 100.
     *               Positive = forward, negative = backward, 0 = brake.
     */
    void motor(MeloMotor motor, int8_t speed);

    // -----------------------------------------------------------------------
    // SERVO APIs
    // -----------------------------------------------------------------------

    /**
     * Set servo angle.
     *
     * @param servoNum  Servo index (1-based). Use 1 for the first servo.
     * @param angle     Target angle in degrees (0-180).
     *                  Clamped to safe limits.
     */
    void servo(uint8_t servoNum, uint8_t angle);

    /**
     * Move servo to center position (90 degrees).
     *
     * @param servoNum  Servo index (1-based).
     */
    void servoCenter(uint8_t servoNum = 1);

    // -----------------------------------------------------------------------
    // SENSOR APIs
    // -----------------------------------------------------------------------

    /**
     * Read distance from ultrasonic sensor.
     *
     * @return  Distance in centimeters. Returns -1.0 if out of range or error.
     *
     * Valid range: 2-400 cm (HC-SR04).
     * Call interval: minimum 50ms between calls recommended.
     * Blocking: ~30ms per call.
     */
    float readDistance();

    /**
     * Read distance with full result struct.
     * @return  MeloDistanceReading with distance, validity, and status.
     */
    MeloDistanceReading readDistanceFull();

    /**
     * Check if an obstacle is closer than a given distance.
     *
     * @param thresholdCm  Distance threshold in centimeters.
     * @return             true if object is within threshold.
     */
    bool obstacleDetected(float thresholdCm = 20.0f);

    /**
     * Read line sensor.
     *
     * @return  true if line detected (dark surface), false otherwise.
     */
    bool readLine();

    /**
     * Read line sensor with full result struct.
     * @return  MeloLineSensorReading with state, raw value, and status.
     */
    MeloLineSensorReading readLineFull();

    /**
     * Read light sensor (analog).
     *
     * @return  Raw ADC value (0-4095 on ESP32). Higher = more light.
     */
    uint16_t readLight();

    // -----------------------------------------------------------------------
    // LED APIs
    // -----------------------------------------------------------------------

    /**
     * Set LED state.
     *
     * @param ledNum  LED index (1-based). Use 1 for the first LED.
     * @param on      true = LED on, false = LED off.
     */
    void led(uint8_t ledNum, bool on);

    /**
     * Blink LED.
     *
     * @param ledNum   LED index (1-based).
     * @param times    Number of blink cycles.
     * @param onMs     On duration in milliseconds.
     * @param offMs    Off duration in milliseconds (defaults to onMs).
     *
     * NOTE: Blocking.
     */
    void ledBlink(uint8_t ledNum, uint8_t times, uint16_t onMs, uint16_t offMs = 0);

    // -----------------------------------------------------------------------
    // BUZZER APIs
    // -----------------------------------------------------------------------

    /**
     * Play a tone for a specified duration.
     *
     * @param frequencyHz  Tone frequency in Hz (20-20000). 0 = silence.
     * @param durationMs   Tone duration in milliseconds.
     *
     * NOTE: Blocking.
     */
    void beep(uint16_t frequencyHz, uint32_t durationMs);

    /**
     * Start playing a continuous tone (non-blocking).
     *
     * @param frequencyHz  Tone frequency in Hz.
     */
    void startBeep(uint16_t frequencyHz);

    /**
     * Stop any currently playing tone.
     */
    void stopBeep();

    // -----------------------------------------------------------------------
    // Internal state (do not use directly in sketches)
    // -----------------------------------------------------------------------

private:
    bool        _initialized;
    MeloStatus  _lastError;

    // Subsystem drivers
    MeloMotorDriver   _leftMotor;
    MeloMotorDriver   _rightMotor;
    MeloDistanceSensor _distanceSensor;
    MeloLineSensor    _lineSensor;
    MeloServoDriver   _servo1;
    MeloLEDDriver     _led1;
    MeloBuzzerDriver  _buzzer;

    // Internal helpers
    void _setError(MeloStatus err);
    void _debugPrint(const String& msg) const;
    bool _checkInit(const char* callerName) const;
};

// ---------------------------------------------------------------------------
// Global Singleton
// Users interact via the global Melo object
// ---------------------------------------------------------------------------
extern MeloClass Melo;

#endif // MELO_H
