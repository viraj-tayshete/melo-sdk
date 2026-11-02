/**
 * MeloWheel.cpp
 *
 * Continuous-rotation servo wheel driver.
 * Requires the ESP32 Arduino core 3.x (pin-based ledcAttach/ledcWrite).
 *
 * SDK Version: 0.2.0
 */

#include "MeloWheel.h"

MeloWheelDriver::MeloWheelDriver(uint8_t pin, bool inverted, int16_t trimUs)
    : _pin(pin), _inverted(inverted), _trimUs(trimUs), _attached(false)
{
}

bool MeloWheelDriver::begin() {
    // Duty starts at 0 (no pulse), so the wheel stays still until commanded.
    _attached = ledcAttach(_pin, MELO_SERVO_FREQ_HZ, MELO_SERVO_RES_BITS);
    return _attached;
}

void MeloWheelDriver::setSpeed(int8_t speed) {
    if (speed == 0) {
        stop();
        return;
    }
    _writePulse(meloWheelPulseUs(speed, _inverted, _trimUs));
}

void MeloWheelDriver::stop() {
#if MELO_STOP_CUTS_PULSE
    if (_attached) ledcWrite(_pin, 0);
#else
    _writePulse(meloWheelPulseUs(0, _inverted, _trimUs));
#endif
}

void MeloWheelDriver::_writePulse(uint16_t pulseUs) {
    if (_attached) ledcWrite(_pin, meloPulseToDuty(pulseUs));
}
