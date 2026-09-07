/**
 * MeloServo.cpp
 *
 * Servo driver implementation.
 * Provides high-precision hardware LEDC PWM control on ESP32 (no external library required)
 * and falls back to standard Arduino Servo library on ESP8266/AVR.
 *
 * SDK Version: 0.1.0
 */

#include "MeloServo.h"

MeloServoDriver::MeloServoDriver(uint8_t pin, uint8_t minAngle, uint8_t maxAngle, uint8_t pwmCh)
    : _pin(pin), _minAngle(minAngle), _maxAngle(maxAngle), _currentAngle(90), _pwmCh(pwmCh), _attached(false)
{
}

void MeloServoDriver::begin() {
#if defined(ESP32)
  #if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    ledcAttachChannel(_pin, 50, 14, _pwmCh);
  #else
    ledcSetup(_pwmCh, 50, 14);
    ledcAttachPin(_pin, _pwmCh);
  #endif
    _attached = true;
    center();
#else
    _servo.attach(_pin, SERVO_1_MIN_PULSE, SERVO_1_MAX_PULSE);
    _attached = true;
    center();
#endif
}

void MeloServoDriver::setAngle(uint8_t angle) {
    angle = MELO_CLAMP(angle, _minAngle, _maxAngle);
    _currentAngle = angle;

#if defined(ESP32)
    if (!_attached) {
      #if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
        ledcAttachChannel(_pin, 50, 14, _pwmCh);
      #else
        ledcAttachPin(_pin, _pwmCh);
      #endif
        _attached = true;
    }
    // Convert angle to pulse width in microseconds (SERVO_1_MIN_PULSE to SERVO_1_MAX_PULSE)
    uint32_t pulseUs = SERVO_1_MIN_PULSE +
        (uint32_t)(SERVO_1_MAX_PULSE - SERVO_1_MIN_PULSE) * (angle - _minAngle) / (_maxAngle - _minAngle);
    // Period is 20000 us (50Hz), resolution is 14-bit (0..16383)
    uint32_t duty = (pulseUs * 16383UL) / 20000UL;
  #if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    ledcWriteChannel(_pwmCh, duty);
  #else
    ledcWrite(_pwmCh, duty);
  #endif
#else
    _servo.write(angle);
#endif
}

void MeloServoDriver::center() {
    setAngle((_minAngle + _maxAngle) / 2);
}

void MeloServoDriver::detach() {
#if defined(ESP32)
  #if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    ledcWriteChannel(_pwmCh, 0);
  #else
    ledcWrite(_pwmCh, 0);
  #endif
    _attached = false;
#else
    _servo.detach();
    _attached = false;
#endif
}

bool MeloServoDriver::isAttached() const {
#if defined(ESP32)
    return _attached;
#else
    return _servo.attached();
#endif
}
