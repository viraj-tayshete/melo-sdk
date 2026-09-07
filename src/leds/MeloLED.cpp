/**
 * MeloLED.cpp
 *
 * LED driver implementation.
 *
 * SDK Version: 0.1.0
 */

#include "MeloLED.h"

MeloLEDDriver::MeloLEDDriver(uint8_t pin, bool activeHigh)
    : _pin(pin), _activeHigh(activeHigh), _isOn(false)
{
}

void MeloLEDDriver::begin() {
    pinMode(_pin, OUTPUT);
    off();  // Safe default state
}

void MeloLEDDriver::set(bool on) {
    _isOn = on;
    _write(on);
}

void MeloLEDDriver::on() {
    set(true);
}

void MeloLEDDriver::off() {
    set(false);
}

void MeloLEDDriver::toggle() {
    set(!_isOn);
}

void MeloLEDDriver::blink(uint8_t times, uint16_t onMs, uint16_t offMs) {
    if (offMs == 0) offMs = onMs;  // Default off = same as on duration

    for (uint8_t i = 0; i < times; i++) {
        on();
        delay(onMs);
        off();
        if (i < times - 1) {
            delay(offMs);  // No final off delay after last blink
        }
    }
}

void MeloLEDDriver::_write(bool state) {
    // Apply active-high / active-low polarity
    digitalWrite(_pin, (_activeHigh ? state : !state) ? HIGH : LOW);
}
