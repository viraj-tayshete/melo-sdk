/**
 * MeloLED.cpp
 *
 * SDK Version: 0.2.0
 */

#include "MeloLED.h"

MeloLEDDriver::MeloLEDDriver(uint8_t pin, bool activeHigh)
    : _pin(pin), _activeHigh(activeHigh), _isOn(false)
{
}

void MeloLEDDriver::begin() {
    pinMode(_pin, OUTPUT);
    set(false);
}

void MeloLEDDriver::set(bool on) {
    _isOn = on;
    digitalWrite(_pin, (_activeHigh ? on : !on) ? HIGH : LOW);
}

void MeloLEDDriver::blink(uint8_t times, uint16_t onMs, uint16_t offMs) {
    if (offMs == 0) offMs = onMs;
    for (uint8_t i = 0; i < times; i++) {
        set(true);
        delay(onMs);
        set(false);
        if (i + 1 < times) delay(offMs);
    }
}
