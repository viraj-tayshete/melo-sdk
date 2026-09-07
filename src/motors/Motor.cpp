/**
 * Motor.cpp
 *
 * Implementation of the Melo single-motor driver.
 *
 * ASSUMPTION: L298N-style H-bridge (EN + IN1/IN2).
 * For DRV8833 / TB6612FNG the direction logic is identical,
 * but these drivers use AIN1/AIN2 as PWM inputs directly.
 * Update _applyPWM() and _setDirection() if driver type changes.
 *
 * SDK Version: 0.1.0
 */

#include "Motor.h"
#if defined(ESP32)
  #include <esp_arduino_version.h>
#endif

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

MeloMotorDriver::MeloMotorDriver(uint8_t pwmPin, uint8_t dir1Pin, uint8_t dir2Pin, uint8_t pwmCh)
    : _pwmPin(pwmPin), _dir1Pin(dir1Pin), _dir2Pin(dir2Pin), _pwmCh(pwmCh), _lastSpeed(0)
{
}

// ---------------------------------------------------------------------------
// Initialization
// ---------------------------------------------------------------------------

void MeloMotorDriver::begin() {
    pinMode(_dir1Pin, OUTPUT);
    pinMode(_dir2Pin, OUTPUT);

#if defined(ESP32)
  #if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    ledcAttachChannel(_pwmPin, MOTOR_PWM_FREQ, MOTOR_PWM_RESOLUTION, _pwmCh);
  #else
    ledcSetup(_pwmCh, MOTOR_PWM_FREQ, MOTOR_PWM_RESOLUTION);
    ledcAttachPin(_pwmPin, _pwmCh);
  #endif
#else
    // ESP8266 / AVR: use analogWrite (software PWM)
    pinMode(_pwmPin, OUTPUT);
#endif

    brake();  // Start in safe stopped state
}

// ---------------------------------------------------------------------------
// Speed Control
// ---------------------------------------------------------------------------

void MeloMotorDriver::setSpeed(int8_t speed) {
    // Clamp speed to valid range
    speed = MELO_CLAMP(speed, -100, 100);
    _lastSpeed = speed;

    if (speed == 0) {
        brake();
        return;
    }

    MeloDirection dir = (speed > 0) ? DIR_FORWARD : DIR_BACKWARD;
    uint8_t pwmValue  = PCT_TO_PWM(abs(speed));

    _setDirection(dir);
    _applyPWM(pwmValue);
}

// ---------------------------------------------------------------------------
// Brake (active stop — holds motor stationary)
// ---------------------------------------------------------------------------

void MeloMotorDriver::brake() {
    _lastSpeed = 0;
    _setDirection(DIR_STOP);
    _applyPWM(0);
}

// ---------------------------------------------------------------------------
// Coast (passive stop — disables drive, motor spins down freely)
// ---------------------------------------------------------------------------

void MeloMotorDriver::coast() {
    _lastSpeed = 0;
    digitalWrite(_dir1Pin, LOW);
    digitalWrite(_dir2Pin, LOW);
    _applyPWM(0);
}

// ---------------------------------------------------------------------------
// Private: Apply PWM duty cycle
// ---------------------------------------------------------------------------

void MeloMotorDriver::_applyPWM(uint8_t dutyCycle) {
#if defined(ESP32)
  #if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    ledcWriteChannel(_pwmCh, dutyCycle);
  #else
    ledcWrite(_pwmCh, dutyCycle);
  #endif
#else
    analogWrite(_pwmPin, dutyCycle);
#endif
}

// ---------------------------------------------------------------------------
// Private: Set direction GPIO pins
//
// L298N / TB6612FNG:
//   FORWARD  -> IN1=HIGH, IN2=LOW
//   BACKWARD -> IN1=LOW,  IN2=HIGH
//   STOP     -> IN1=LOW,  IN2=LOW  (coast) or IN1=HIGH, IN2=HIGH (brake)
//
// DRV8833:
//   Logic is the same; AIN1/AIN2 are driven with PWM instead of a separate EN pin.
//   This needs adjustment if MOTOR_DRIVER_TYPE == MELO_DRIVER_DRV8833.
// ---------------------------------------------------------------------------

void MeloMotorDriver::_setDirection(MeloDirection dir) {
    switch (dir) {
        case DIR_FORWARD:
            digitalWrite(_dir1Pin, HIGH);
            digitalWrite(_dir2Pin, LOW);
            break;
        case DIR_BACKWARD:
            digitalWrite(_dir1Pin, LOW);
            digitalWrite(_dir2Pin, HIGH);
            break;
        case DIR_STOP:
        default:
            // Brake: both LOW for coast, both HIGH for active brake
            // Using coast-style (both LOW) as default safe behavior
            digitalWrite(_dir1Pin, LOW);
            digitalWrite(_dir2Pin, LOW);
            break;
    }
}
