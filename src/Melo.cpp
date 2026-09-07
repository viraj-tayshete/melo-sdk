/**
 * Melo.cpp
 *
 * Melo SDK — Main API Implementation
 *
 * This file wires all subsystem drivers to the public Melo API.
 *
 * SDK Version: 0.1.0
 */

#include "Melo.h"

// ---------------------------------------------------------------------------
// Global Singleton Definition
// ---------------------------------------------------------------------------
MeloClass Melo;

// ---------------------------------------------------------------------------
// Constructor — Initialize drivers with pin config from MeloConfig.h
// ---------------------------------------------------------------------------

MeloClass::MeloClass()
    : _initialized(false),
      _lastError(MELO_OK),
      _leftMotor(MOTOR_LEFT_PWM_PIN,  MOTOR_LEFT_DIR1_PIN,  MOTOR_LEFT_DIR2_PIN,  MOTOR_LEFT_PWM_CH),
      _rightMotor(MOTOR_RIGHT_PWM_PIN, MOTOR_RIGHT_DIR1_PIN, MOTOR_RIGHT_DIR2_PIN, MOTOR_RIGHT_PWM_CH),
      _distanceSensor(ULTRASONIC_TRIGGER_PIN, ULTRASONIC_ECHO_PIN),
      _lineSensor(LINE_SENSOR_PIN, LINE_SENSOR_ANALOG),
      _servo1(SERVO_1_PIN),
      _led1(LED_1_PIN, true),         // Active HIGH assumed
      _buzzer(BUZZER_PIN, BUZZER_PWM_CH)
{
}

// ---------------------------------------------------------------------------
// begin()
// ---------------------------------------------------------------------------

MeloStatus MeloClass::begin() {
    // 1. Initialize serial output for diagnostics
    Serial.begin(MELO_SERIAL_BAUD);
    delay(100);  // Allow serial to settle

    _debugPrint("Melo SDK v" + String(MELO_SDK_VERSION) + " initializing...");

    // 2. Initialize motor subsystem
    _leftMotor.begin();
    _rightMotor.begin();
    _debugPrint("[OK] Motors initialized");

    // 3. Initialize sensor subsystems
    _distanceSensor.begin();
    _lineSensor.begin();
    _debugPrint("[OK] Sensors initialized");

    // 4. Initialize servo
    _servo1.begin();
    _debugPrint("[OK] Servo initialized");

    // 5. Initialize LED
    _led1.begin();
    _debugPrint("[OK] LED initialized");

    // 6. Initialize buzzer
    _buzzer.begin();
    _debugPrint("[OK] Buzzer initialized");

    // 7. Play startup tone to confirm successful initialization
    _buzzer.playStartupTone();

    // 8. Blink LED to confirm ready state
    _led1.blink(2, 200, 100);

    _initialized = true;
    _lastError = MELO_OK;

    _debugPrint("Melo ready!");
    printStatus();

    return MELO_OK;
}

// ---------------------------------------------------------------------------
// SYSTEM INFO
// ---------------------------------------------------------------------------

String MeloClass::getSdkVersion() const {
    return String(MELO_SDK_VERSION);
}

String MeloClass::getFirmwareVersion() const {
    // For Model A (direct Arduino), firmware version = SDK version
    // This will be updated in Step 3 to query actual firmware
    return String(MELO_SDK_VERSION);
}

MeloDeviceInfo MeloClass::getDeviceInfo() const {
    MeloDeviceInfo info;
    info.hwVersion       = String(MELO_HW_VERSION);
    info.firmwareVersion = getFirmwareVersion();
    info.protocolVersion = MELO_PROTOCOL_VERSION;
    info.sdkVersion      = getSdkVersion();
    info.isCompatible    = true;  // In Model A, always compatible
    return info;
}

String MeloClass::lastErrorMessage() const {
    return statusMessage(_lastError);
}

String MeloClass::statusMessage(MeloStatus status) const {
    switch (status) {
        case MELO_OK:                   return "OK — Melo is ready";
        case MELO_ERR_NOT_INIT:         return "Error: Melo.begin() has not been called. Call begin() in setup() first.";
        case MELO_ERR_DEVICE_NOT_FOUND: return "Error: Melo board not detected. Check USB connection and try again.";
        case MELO_ERR_TIMEOUT:          return "Error: Operation timed out. Check hardware connections.";
        case MELO_ERR_FIRMWARE_MISMATCH:return "Error: Firmware version is not compatible with this SDK. Please update firmware.";
        case MELO_ERR_PROTOCOL_MISMATCH:return "Error: Protocol version mismatch. Update SDK or firmware.";
        case MELO_ERR_INVALID_ARG:      return "Error: Invalid argument. Check parameter ranges.";
        case MELO_ERR_INVALID_CMD:      return "Error: Invalid command.";
        case MELO_ERR_SENSOR:           return "Error: Sensor read failed. Check sensor connections.";
        case MELO_ERR_CALIBRATION:      return "Error: Calibration failed. Try again or check hardware.";
        case MELO_ERR_MOTOR:            return "Error: Motor error. Check motor and driver connections.";
        case MELO_ERR_TRANSPORT:        return "Error: Communication transport failure.";
        default:                        return "Error: Unknown error (" + String((int)status) + ")";
    }
}

void MeloClass::printStatus() const {
    MeloDeviceInfo info = getDeviceInfo();
    Serial.println("-------- Melo Status --------");
    Serial.println("SDK Version:      " + info.sdkVersion);
    Serial.println("Hardware Version: " + info.hwVersion);
    Serial.println("Protocol Version: " + String(info.protocolVersion));
    Serial.println("Initialized:      " + String(_initialized ? "YES" : "NO"));
    Serial.println("Last Error:       " + lastErrorMessage());
    Serial.println("-----------------------------");
}

// ---------------------------------------------------------------------------
// ROBOT-LEVEL MOVEMENT APIs
// ---------------------------------------------------------------------------

void MeloClass::moveForward(uint8_t speed) {
    if (!_checkInit("moveForward")) return;
    speed = MELO_CLAMP(speed, 0, 100);
    _leftMotor.setSpeed((int8_t)speed);
    _rightMotor.setSpeed((int8_t)speed);
}

void MeloClass::moveBackward(uint8_t speed) {
    if (!_checkInit("moveBackward")) return;
    speed = MELO_CLAMP(speed, 0, 100);
    _leftMotor.setSpeed(-(int8_t)speed);
    _rightMotor.setSpeed(-(int8_t)speed);
}

void MeloClass::turnLeft(uint8_t speed) {
    if (!_checkInit("turnLeft")) return;
    speed = MELO_CLAMP(speed, 0, 100);
    // Differential turn: right motor forward, left motor backward
    _leftMotor.setSpeed(-(int8_t)speed);
    _rightMotor.setSpeed((int8_t)speed);
}

void MeloClass::turnRight(uint8_t speed) {
    if (!_checkInit("turnRight")) return;
    speed = MELO_CLAMP(speed, 0, 100);
    // Differential turn: left motor forward, right motor backward
    _leftMotor.setSpeed((int8_t)speed);
    _rightMotor.setSpeed(-(int8_t)speed);
}

void MeloClass::stop() {
    if (!_checkInit("stop")) return;
    _leftMotor.brake();
    _rightMotor.brake();
}

void MeloClass::coast() {
    if (!_checkInit("coast")) return;
    _leftMotor.coast();
    _rightMotor.coast();
}

// ---------------------------------------------------------------------------
// COMPONENT-LEVEL MOTOR API
// ---------------------------------------------------------------------------

void MeloClass::motor(MeloMotor m, int8_t speed) {
    if (!_checkInit("motor")) return;
    speed = MELO_CLAMP(speed, -100, 100);
    switch (m) {
        case MOTOR_LEFT:
            _leftMotor.setSpeed(speed);
            break;
        case MOTOR_RIGHT:
            _rightMotor.setSpeed(speed);
            break;
        case MOTOR_BOTH:
            _leftMotor.setSpeed(speed);
            _rightMotor.setSpeed(speed);
            break;
    }
}

// ---------------------------------------------------------------------------
// SERVO APIs
// ---------------------------------------------------------------------------

void MeloClass::servo(uint8_t servoNum, uint8_t angle) {
    if (!_checkInit("servo")) return;
    // Only servo 1 is supported in v0.1.0
    if (servoNum == 1) {
        _servo1.setAngle(angle);
    } else {
        _setError(MELO_ERR_INVALID_ARG);
        _debugPrint("servo(): Invalid servo number " + String(servoNum) + ". Only servo 1 is supported.");
    }
}

void MeloClass::servoCenter(uint8_t servoNum) {
    if (!_checkInit("servoCenter")) return;
    if (servoNum == 1) {
        _servo1.center();
    }
}

// ---------------------------------------------------------------------------
// SENSOR APIs
// ---------------------------------------------------------------------------

float MeloClass::readDistance() {
    if (!_checkInit("readDistance")) return -1.0f;
    return _distanceSensor.readCm();
}

MeloDistanceReading MeloClass::readDistanceFull() {
    MeloDistanceReading err_result;
    if (!_checkInit("readDistanceFull")) {
        err_result.distanceCm = -1.0f;
        err_result.valid = false;
        err_result.status = MELO_ERR_NOT_INIT;
        return err_result;
    }
    return _distanceSensor.read();
}

bool MeloClass::obstacleDetected(float thresholdCm) {
    if (!_checkInit("obstacleDetected")) return false;
    return _distanceSensor.isCloserThan(thresholdCm);
}

bool MeloClass::readLine() {
    if (!_checkInit("readLine")) return false;
    return _lineSensor.onLine();
}

MeloLineSensorReading MeloClass::readLineFull() {
    MeloLineSensorReading err_result;
    if (!_checkInit("readLineFull")) {
        err_result.onLine = false;
        err_result.rawValue = 0;
        err_result.valid = false;
        err_result.status = MELO_ERR_NOT_INIT;
        return err_result;
    }
    return _lineSensor.read();
}

uint16_t MeloClass::readLight() {
    if (!_checkInit("readLight")) return 0;
    return (uint16_t)analogRead(LIGHT_SENSOR_PIN);
}

// ---------------------------------------------------------------------------
// LED APIs
// ---------------------------------------------------------------------------

void MeloClass::led(uint8_t ledNum, bool on) {
    if (!_checkInit("led")) return;
    if (ledNum == 1) {
        _led1.set(on);
    } else {
        _setError(MELO_ERR_INVALID_ARG);
        _debugPrint("led(): Invalid LED number " + String(ledNum));
    }
}

void MeloClass::ledBlink(uint8_t ledNum, uint8_t times, uint16_t onMs, uint16_t offMs) {
    if (!_checkInit("ledBlink")) return;
    if (ledNum == 1) {
        _led1.blink(times, onMs, offMs);
    }
}

// ---------------------------------------------------------------------------
// BUZZER APIs
// ---------------------------------------------------------------------------

void MeloClass::beep(uint16_t frequencyHz, uint32_t durationMs) {
    if (!_checkInit("beep")) return;
    _buzzer.beep(frequencyHz, durationMs);
}

void MeloClass::startBeep(uint16_t frequencyHz) {
    if (!_checkInit("startBeep")) return;
    _buzzer.startTone(frequencyHz);
}

void MeloClass::stopBeep() {
    if (!_checkInit("stopBeep")) return;
    _buzzer.stopTone();
}

// ---------------------------------------------------------------------------
// Private Helpers
// ---------------------------------------------------------------------------

void MeloClass::_setError(MeloStatus err) {
    _lastError = err;
}

void MeloClass::_debugPrint(const String& msg) const {
    if (MELO_DEBUG_ENABLED) {
        Serial.println("[Melo] " + msg);
    }
}

bool MeloClass::_checkInit(const char* callerName) const {
    if (!_initialized) {
        Serial.println("[Melo] ERROR: " + String(callerName) +
                       "() called before Melo.begin(). Call begin() in setup() first.");
        return false;
    }
    return true;
}
