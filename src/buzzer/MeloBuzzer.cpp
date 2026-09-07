/**
 * MeloBuzzer.cpp
 *
 * Passive buzzer implementation.
 *
 * Uses tone()/noTone() on ESP8266 and AVR.
 * Uses LEDC hardware PWM on ESP32.
 *
 * SDK Version: 0.1.0
 */

#include "MeloBuzzer.h"

MeloBuzzerDriver::MeloBuzzerDriver(uint8_t pin, uint8_t pwmCh)
    : _pin(pin), _pwmCh(pwmCh), _isPlaying(false)
{
}

void MeloBuzzerDriver::begin() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);

#if defined(ESP32)
    // Pre-configure LEDC channel at low resolution for tone generation
    // Will be reconfigured per frequency on each beep
    ledcSetup(_pwmCh, 1000, 8);
    ledcAttachPin(_pin, _pwmCh);
    ledcWrite(_pwmCh, 0);  // Silent
#endif
}

void MeloBuzzerDriver::beep(uint16_t frequencyHz, uint32_t durationMs) {
    if (frequencyHz == 0) {
        // Silence for given duration
        stopTone();
        delay(durationMs);
        return;
    }

    startTone(frequencyHz);
    delay(durationMs);
    stopTone();
}

void MeloBuzzerDriver::startTone(uint16_t frequencyHz) {
    _isPlaying = true;
    _startPWM(frequencyHz);
}

void MeloBuzzerDriver::stopTone() {
    _isPlaying = false;
    _stopPWM();
}

void MeloBuzzerDriver::playStartupTone() {
    // Two ascending beeps indicating ready
    beep(880, 100);
    delay(50);
    beep(1320, 150);
}

void MeloBuzzerDriver::playErrorTone() {
    // Three descending beeps indicating error
    beep(880, 200);
    delay(50);
    beep(660, 200);
    delay(50);
    beep(440, 400);
}

void MeloBuzzerDriver::_startPWM(uint16_t frequencyHz) {
#if defined(ESP32)
    // Reconfigure LEDC channel with new frequency
    ledcSetup(_pwmCh, frequencyHz, 8);
    ledcAttachPin(_pin, _pwmCh);
    ledcWrite(_pwmCh, 128);  // 50% duty cycle for passive buzzer
#else
    // ESP8266 / AVR: use Arduino tone()
    tone(_pin, frequencyHz);
#endif
}

void MeloBuzzerDriver::_stopPWM() {
#if defined(ESP32)
    ledcWrite(_pwmCh, 0);
#else
    noTone(_pin);
    digitalWrite(_pin, LOW);
#endif
}
