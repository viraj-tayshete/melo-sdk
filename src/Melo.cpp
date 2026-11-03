/**
 * Melo.cpp
 *
 * Melo SDK - main API implementation. Wires the drivers to the public API.
 *
 * SDK Version: 0.2.0
 */

#include "Melo.h"
#include "PinRules.h"

// Two parts sharing a pin would silently fight each other.
static constexpr bool meloPinsDistinct(const int* pins, int n) {
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (pins[i] == pins[j]) return false;
    return true;
}
static constexpr int kMeloPins[] = {
    MELO_LEFT_WHEEL_PIN, MELO_RIGHT_WHEEL_PIN,
    MELO_PORT1_SIGNAL_PIN, MELO_PORT2_SIGNAL_PIN, MELO_PORT3_SIGNAL_PIN,
    MELO_PORT4_SIGNAL_PIN, MELO_PORT5_SIGNAL_PIN, MELO_PORT6_SIGNAL_PIN,
    MELO_PORT1_ADD_PIN, MELO_PORT2_ADD_PIN, MELO_PORT3_ADD_PIN,
    MELO_PORT4_ADD_PIN, MELO_PORT5_ADD_PIN, MELO_PORT6_ADD_PIN
};
static_assert(meloPinsDistinct(kMeloPins, sizeof(kMeloPins) / sizeof(kMeloPins[0])),
              "Melo: two wheel or port pins in MeloConfig.h are the same pin");

// Every pin must be a GPIO the ESP32-S3-WROOM can safely use (see PinRules.h).
#ifndef MELO_ALLOW_RESERVED_PINS
static constexpr bool meloPinsUsable(const int* pins, int n) {
    for (int i = 0; i < n; i++)
        if (!meloGpioUsable(pins[i])) return false;
    return true;
}
static_assert(meloPinsUsable(kMeloPins, sizeof(kMeloPins) / sizeof(kMeloPins[0])),
              "Melo: a pin in MeloConfig.h is reserved or missing on the ESP32-S3-WROOM (flash, USB, strapping, console). See PinRules.h.");
#endif

// Each part must sit on its own port.
static_assert(MELO_IR_PORT >= 1 && MELO_IR_PORT <= MELO_PORT_COUNT &&
              MELO_LIGHT_PORT >= 1 && MELO_LIGHT_PORT <= MELO_PORT_COUNT &&
              MELO_LED_PORT >= 1 && MELO_LED_PORT <= MELO_PORT_COUNT &&
              MELO_BUZZER_PORT >= 1 && MELO_BUZZER_PORT <= MELO_PORT_COUNT,
              "Melo: a part is assigned to a port outside 1-6");
static_assert(MELO_IR_PORT != MELO_LIGHT_PORT && MELO_IR_PORT != MELO_LED_PORT &&
              MELO_IR_PORT != MELO_BUZZER_PORT && MELO_LIGHT_PORT != MELO_LED_PORT &&
              MELO_LIGHT_PORT != MELO_BUZZER_PORT && MELO_LED_PORT != MELO_BUZZER_PORT,
              "Melo: two parts are assigned to the same port");

static const uint8_t kMeloAddPins[MELO_PORT_COUNT] = {
    MELO_PORT1_ADD_PIN, MELO_PORT2_ADD_PIN, MELO_PORT3_ADD_PIN,
    MELO_PORT4_ADD_PIN, MELO_PORT5_ADD_PIN, MELO_PORT6_ADD_PIN
};

MeloClass Melo;

MeloClass::MeloClass()
    : _initialized(false),
      _lastError(MELO_OK),
      _leftWheel(MELO_LEFT_WHEEL_PIN,   MELO_LEFT_WHEEL_INVERTED,  MELO_LEFT_WHEEL_TRIM_US),
      _rightWheel(MELO_RIGHT_WHEEL_PIN, MELO_RIGHT_WHEEL_INVERTED, MELO_RIGHT_WHEEL_TRIM_US),
      _ir(MELO_IR_PIN),
      _led(MELO_LED_PIN, MELO_LED_ACTIVE_HIGH),
      _buzzer(MELO_BUZZER_PIN),
      _lastFollowMs(0)
{
}

// ---------------------------------------------------------------------------
// SYSTEM
// ---------------------------------------------------------------------------

MeloStatus MeloClass::begin() {
    if (_initialized) return MELO_OK;

    Serial.begin(MELO_SERIAL_BAUD);   // does not wait for a serial monitor
    delay(100);
    _debugPrint(F("Melo SDK v" MELO_SDK_VERSION " starting..."));

    _buzzer.begin();
    _led.begin();

    if (!_leftWheel.begin() || !_rightWheel.begin()) {
        _leftWheel.stop();
        _rightWheel.stop();
        _setError(MELO_ERR_PIN);
        _debugPrint(F("[FAIL] A wheel pin could not be set up. Check MELO_LEFT_WHEEL_PIN / MELO_RIGHT_WHEEL_PIN."));
        _buzzer.playErrorTone();
        return MELO_ERR_PIN;
    }

    _ir.begin();
    pinMode(MELO_LIGHT_PIN, INPUT);
    for (uint8_t i = 0; i < MELO_PORT_COUNT; i++) pinMode(kMeloAddPins[i], INPUT);

    _buzzer.playStartupTone();
    _led.blink(2, 200, 100);

    _initialized = true;
    _lastError = MELO_OK;
    _debugPrint(F("Melo ready."));
    return MELO_OK;
}

String MeloClass::getSdkVersion() const {
    return String(MELO_SDK_VERSION);
}

MeloDeviceInfo MeloClass::getDeviceInfo() const {
    MeloDeviceInfo info;
    info.sdkVersion = getSdkVersion();
    info.hwVersion  = String(MELO_HW_VERSION);
#ifdef ARDUINO_BOARD
    info.board      = String(ARDUINO_BOARD);
#else
    info.board      = String("unknown");
#endif
    return info;
}

String MeloClass::lastErrorMessage() const {
    return statusMessage(_lastError);
}

String MeloClass::statusMessage(MeloStatus status) const {
    switch (status) {
        case MELO_OK:
            return F("OK - Melo is ready");
        case MELO_ERR_NOT_INIT:
            return F("Melo is not set up. Call Melo.begin() first in setup().");
        case MELO_ERR_INVALID_ARG:
            return F("A value was out of range. Check the allowed range for that command.");
        case MELO_ERR_PIN:
            return F("A pin could not be set up. Check the wiring and the pin settings.");
        case MELO_ERR_LINE_LOST:
            return F("The line was lost. Put the robot back on the line and try again.");
        default:
            return "Unknown error (" + String((int)status) + ")";
    }
}

void MeloClass::printStatus() const {
    MeloDeviceInfo info = getDeviceInfo();
    Serial.println(F("-------- Melo Status --------"));
    Serial.println("SDK version:  " + info.sdkVersion);
    Serial.println("Hardware:     " + info.hwVersion);
    Serial.println("Board:        " + info.board);
    Serial.println(String("Ready:        ") + (_initialized ? "YES" : "NO"));
    Serial.println("Last error:   " + lastErrorMessage());
    Serial.println(F("-----------------------------"));
}

// ---------------------------------------------------------------------------
// DRIVING
// ---------------------------------------------------------------------------

void MeloClass::moveForward(uint8_t speed) {
    if (!_checkInit("moveForward")) return;
    int8_t s = (int8_t)MELO_CLAMP((int)speed, 0, 100);
    _leftWheel.setSpeed(s);
    _rightWheel.setSpeed(s);
}

void MeloClass::moveBackward(uint8_t speed) {
    if (!_checkInit("moveBackward")) return;
    int8_t s = (int8_t)MELO_CLAMP((int)speed, 0, 100);
    _leftWheel.setSpeed(-s);
    _rightWheel.setSpeed(-s);
}

void MeloClass::turnLeft(uint8_t speed) {
    if (!_checkInit("turnLeft")) return;
    int8_t s = (int8_t)MELO_CLAMP((int)speed, 0, 100);
    _leftWheel.setSpeed(-s);
    _rightWheel.setSpeed(s);
}

void MeloClass::turnRight(uint8_t speed) {
    if (!_checkInit("turnRight")) return;
    int8_t s = (int8_t)MELO_CLAMP((int)speed, 0, 100);
    _leftWheel.setSpeed(s);
    _rightWheel.setSpeed(-s);
}

void MeloClass::stop() {
    if (!_checkInit("stop")) return;
    _leftWheel.stop();
    _rightWheel.stop();
}

void MeloClass::wheel(MeloWheelSide side, int8_t speed) {
    if (!_checkInit("wheel")) return;
    speed = (int8_t)MELO_CLAMP((int)speed, -100, 100);
    if (side == WHEEL_LEFT  || side == WHEEL_BOTH) _leftWheel.setSpeed(speed);
    if (side == WHEEL_RIGHT || side == WHEEL_BOTH) _rightWheel.setSpeed(speed);
}

void MeloClass::trimWheel(MeloWheelSide side, int16_t trimUs) {
    if (!_checkInit("trimWheel")) return;
    trimUs = (int16_t)MELO_CLAMP((int)trimUs, -200, 200);
    if (side == WHEEL_LEFT  || side == WHEEL_BOTH) _leftWheel.setTrimUs(trimUs);
    if (side == WHEEL_RIGHT || side == WHEEL_BOTH) _rightWheel.setTrimUs(trimUs);
}

bool MeloClass::followLine(uint8_t speed) {
    if (!_checkInit("followLine")) return false;

    uint32_t now = millis();
    if (_lastFollowMs != 0 && (uint32_t)(now - _lastFollowMs) > MELO_LINE_NEW_RUN_GAP_MS) {
        _lineLogic.reset();
    }
    _lastFollowMs = now;

    MeloDriveCmd cmd = _lineLogic.step(lineDetected(), now, speed);
    if (_lineLogic.state() == LINE_GAVE_UP) {
        _leftWheel.stop();
        _rightWheel.stop();
        _setError(MELO_ERR_LINE_LOST);
        _buzzer.playErrorTone();
        _lastFollowMs = 0;
        _lineLogic.reset();
        return false;
    }
    _leftWheel.setSpeed(cmd.left);
    _rightWheel.setSpeed(cmd.right);
    delay(MELO_LINE_STEP_MS);   // also lets the ESP32 background tasks run
    return true;
}

void MeloClass::resetLineFollow() {
    _lineLogic.reset();
    _lastFollowMs = 0;
}

void MeloClass::halt() {
    _leftWheel.stop();
    _rightWheel.stop();
    _buzzer.stopTone();
    _led.set(false);
    while (true) {
        delay(1000);
    }
}

// ---------------------------------------------------------------------------
// IR SENSOR
// ---------------------------------------------------------------------------

bool MeloClass::irDetected() {
    if (!_checkInit("irDetected")) return false;
    return _ir.reflecting();
}

bool MeloClass::lineDetected() {
    if (!_checkInit("lineDetected")) return false;
    return !_ir.reflecting();
}

bool MeloClass::obstacleDetected() {
    if (!_checkInit("obstacleDetected")) return false;
    return _ir.reflecting();
}

uint16_t MeloClass::irRaw() {
    if (!_checkInit("irRaw")) return 0;
    return _ir.raw();
}

// ---------------------------------------------------------------------------
// PORTS
// ---------------------------------------------------------------------------

uint16_t MeloClass::portRaw(uint8_t port) {
    if (!_checkInit("portRaw")) return 0;
    if (port < 1 || port > MELO_PORT_COUNT) {
        _setError(MELO_ERR_INVALID_ARG);
        return 0;
    }
    return (uint16_t)analogRead(kMeloAddPins[port - 1]);
}

int32_t MeloClass::portOhms(uint8_t port) {
    if (!_checkInit("portOhms")) return -1;
    if (port < 1 || port > MELO_PORT_COUNT) {
        _setError(MELO_ERR_INVALID_ARG);
        return -1;
    }
    return meloPortOhms((uint16_t)analogRead(kMeloAddPins[port - 1]));
}

// ---------------------------------------------------------------------------
// LIGHT SENSOR
// ---------------------------------------------------------------------------

uint16_t MeloClass::readLightRaw() {
    if (!_checkInit("readLightRaw")) return 0;
    return (uint16_t)analogRead(MELO_LIGHT_PIN);
}

uint8_t MeloClass::readLight() {
    if (!_checkInit("readLight")) return 0;
    uint32_t raw = (uint32_t)analogRead(MELO_LIGHT_PIN);
    if (raw > MELO_ADC_MAX) raw = MELO_ADC_MAX;
    uint8_t pct = (uint8_t)(raw * 100 / MELO_ADC_MAX);
#if MELO_LIGHT_BRIGHT_IS_HIGH
    return pct;
#else
    return (uint8_t)(100 - pct);
#endif
}

// ---------------------------------------------------------------------------
// LED
// ---------------------------------------------------------------------------

void MeloClass::led(bool on) {
    if (!_checkInit("led")) return;
    _led.set(on);
}

void MeloClass::ledBlink(uint8_t times, uint16_t onMs, uint16_t offMs) {
    if (!_checkInit("ledBlink")) return;
    _led.blink(times, onMs, offMs);
}

// ---------------------------------------------------------------------------
// BUZZER
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
// Private helpers
// ---------------------------------------------------------------------------

void MeloClass::_setError(MeloStatus err) {
    _lastError = err;
}

void MeloClass::_debugPrint(const String& msg) const {
    if (MELO_DEBUG_ENABLED) {
        Serial.println("[Melo] " + msg);
    }
}

bool MeloClass::_checkInit(const char* callerName) {
    if (!_initialized) {
        _setError(MELO_ERR_NOT_INIT);
        Serial.print(F("[Melo] "));
        Serial.print(callerName);
        Serial.println(F("() was called before Melo.begin(). Put Melo.begin() first in setup()."));
        return false;
    }
    return true;
}
