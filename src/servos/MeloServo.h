/**
 * MeloServo.h
 *
 * Servo driver for the Melo robotics platform.
 * Wraps the Arduino Servo library with Melo-specific safety limits and indexing.
 *
 * ASSUMPTION: Standard 50Hz PWM servo. Range 0-180 degrees.
 * Verify physical servo limits before commanding to extremes.
 *
 * SDK Version: 0.1.0
 */

#ifndef MELO_SERVO_H
#define MELO_SERVO_H

#include <Arduino.h>
#include "../MeloConfig.h"
#include "../MeloTypes.h"

#if defined(ESP32)
  #include <esp_arduino_version.h>
#else
  #include <Servo.h>
#endif

class MeloServoDriver {
public:
    /**
     * @param pin       GPIO pin connected to servo signal wire
     * @param minAngle  Minimum safe angle in degrees (default 0)
     * @param maxAngle  Maximum safe angle in degrees (default 180)
     * @param pwmCh     LEDC channel on ESP32 (default SERVO_1_PWM_CH)
     */
    MeloServoDriver(uint8_t pin, uint8_t minAngle = 0, uint8_t maxAngle = 180, uint8_t pwmCh = SERVO_1_PWM_CH);

    /** Attach servo to pin. Must be called before use. */
    void begin();

    /**
     * Move servo to target angle.
     *
     * @param angle  Target angle in degrees (0-180).
     *               Clamped to [minAngle, maxAngle] if out of range.
     */
    void setAngle(uint8_t angle);

    /**
     * Move servo to center position (90 degrees by default).
     */
    void center();

    /**
     * Detach servo from PWM signal (disables holding torque).
     * Use when servo is not actively needed to avoid jitter/heat.
     */
    void detach();

    /** Returns the last commanded angle. */
    uint8_t getAngle() const { return _currentAngle; }

    /** Returns true if servo is currently attached. */
    bool isAttached() const;

private:
    uint8_t _pin;
    uint8_t _minAngle;
    uint8_t _maxAngle;
    uint8_t _currentAngle;
    uint8_t _pwmCh;
    bool    _attached;

#if !defined(ESP32)
    Servo   _servo;
#endif
};

#endif // MELO_SERVO_H
