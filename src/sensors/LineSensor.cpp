/**
 * LineSensor.cpp
 *
 * Line sensor (IR reflective, TCRT5000 assumed) implementation.
 *
 * SDK Version: 0.1.0
 */

#include "LineSensor.h"

// Default analog threshold: values below this = line detected (dark surface)
#define DEFAULT_LINE_THRESHOLD  500

MeloLineSensor::MeloLineSensor(uint8_t pin, bool analogMode)
    : _pin(pin), _analogMode(analogMode), _threshold(DEFAULT_LINE_THRESHOLD)
{
}

void MeloLineSensor::begin() {
    if (_analogMode) {
        // Analog pins don't need pinMode on ESP32 ADC, but set INPUT for safety
        pinMode(_pin, INPUT);
    } else {
        // Digital mode: use INPUT_PULLUP if no external pull-up resistor
        // ASSUMPTION: External pull-up present on Melo PCB; using INPUT here
        // Change to INPUT_PULLUP if floating reads are observed
        pinMode(_pin, INPUT);
    }
}

bool MeloLineSensor::onLine() {
    if (_analogMode) {
        return (analogRead(_pin) < _threshold);
    } else {
        // TCRT5000 digital output: LOW = line/dark surface detected
        return (digitalRead(_pin) == LOW);
    }
}

uint16_t MeloLineSensor::rawValue() {
    if (_analogMode) {
        return (uint16_t)analogRead(_pin);
    } else {
        return (uint16_t)digitalRead(_pin);
    }
}

MeloLineSensorReading MeloLineSensor::read() {
    MeloLineSensorReading result;
    result.rawValue = rawValue();
    result.onLine   = onLine();
    result.valid    = true;
    result.status   = MELO_OK;
    return result;
}

void MeloLineSensor::setThreshold(uint16_t threshold) {
    _threshold = threshold;
}
