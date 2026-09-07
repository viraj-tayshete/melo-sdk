/**
 * Motor.h
 *
 * Low-level motor driver abstraction for the Melo robotics platform.
 *
 * Supports H-bridge driver ICs (L298N, DRV8833, TB6612FNG).
 * All pin assignments come from MeloConfig.h — do not hard-code pins here.
 *
 * SDK Version: 0.1.0
 *
 * ASSUMPTION: L298N-style driver (EN + IN1/IN2 per channel).
 * Update MOTOR_DRIVER_TYPE in MeloConfig.h if using DRV8833 or TB6612FNG.
 */

#ifndef MELO_MOTOR_H
#define MELO_MOTOR_H

#include <Arduino.h>
#include "../MeloConfig.h"
#include "../MeloTypes.h"

class MeloMotorDriver {
public:
    /**
     * Initialize a single motor channel.
     *
     * @param pwmPin   GPIO pin connected to motor PWM / enable input
     * @param dir1Pin  GPIO pin for direction IN1 (or AIN1)
     * @param dir2Pin  GPIO pin for direction IN2 (or AIN2)
     * @param pwmCh    ESP32 LEDC PWM channel (0-15); ignored on ESP8266
     */
    MeloMotorDriver(uint8_t pwmPin, uint8_t dir1Pin, uint8_t dir2Pin, uint8_t pwmCh);

    /** Configure GPIO and PWM hardware. Must be called before use. */
    void begin();

    /**
     * Set motor speed and direction.
     *
     * @param speed      Speed percentage 0-100. Negative = backward.
     *                   0 = brake (active stop).
     */
    void setSpeed(int8_t speed);

    /** Actively brake the motor (hold output at 0). */
    void brake();

    /** Coast the motor (disable drive, let it spin down freely). */
    void coast();

    /** Get last set speed (-100 to 100). */
    int8_t getSpeed() const { return _lastSpeed; }

private:
    uint8_t _pwmPin;
    uint8_t _dir1Pin;
    uint8_t _dir2Pin;
    uint8_t _pwmCh;
    int8_t  _lastSpeed;

    void _applyPWM(uint8_t dutyCycle);
    void _setDirection(MeloDirection dir);
};

#endif // MELO_MOTOR_H
