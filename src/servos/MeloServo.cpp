/**
 * MeloServo.cpp
 *
 * Servo driver implementation.
 *
 * SDK Version: 0.1.0
 */

#include "MeloServo.h"

MeloServoDriver::MeloServoDriver(uint8_t pin, uint8_t minAngle, uint8_t maxAngle)
    : _pin(pin), _minAngle(minAngle), _maxAngle(maxAngle), _currentAngle(90)
{
}

void MeloServoDriver::begin() {
    _servo.attach(_pin, SERVO_1_MIN_PULSE, SERVO_1_MAX_PULSE);
    center();  // Move to safe center position on init
}

void MeloServoDriver::setAngle(uint8_t angle) {
    // Clamp to safe range
    angle = MELO_CLAMP(angle, _minAngle, _maxAngle);
    _currentAngle = angle;
    _servo.write(angle);
}

void MeloServoDriver::center() {
    setAngle((_minAngle + _maxAngle) / 2);
}

void MeloServoDriver::detach() {
    _servo.detach();
}

bool MeloServoDriver::isAttached() const {
    return _servo.attached();
}
