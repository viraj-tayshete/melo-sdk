/**
 * MeloBuzzer.cpp
 *
 * SDK Version: 0.2.0
 */

#include "MeloBuzzer.h"

MeloBuzzerDriver::MeloBuzzerDriver(uint8_t pin) : _pin(pin) {
}

void MeloBuzzerDriver::begin() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void MeloBuzzerDriver::beep(uint16_t frequencyHz, uint32_t durationMs) {
    if (frequencyHz == 0) {
        stopTone();
    } else {
        startTone(frequencyHz);
    }
    delay(durationMs);
    stopTone();
}

void MeloBuzzerDriver::startTone(uint16_t frequencyHz) {
    frequencyHz = MELO_CLAMP(frequencyHz, MELO_BUZZER_MIN_HZ, MELO_BUZZER_MAX_HZ);
    tone(_pin, frequencyHz);
}

void MeloBuzzerDriver::stopTone() {
    noTone(_pin);
    digitalWrite(_pin, LOW);
}

void MeloBuzzerDriver::playStartupTone() {
    beep(880, 100);
    delay(50);
    beep(1320, 150);
}

void MeloBuzzerDriver::playErrorTone() {
    beep(880, 200);
    delay(50);
    beep(660, 200);
    delay(50);
    beep(440, 400);
}
