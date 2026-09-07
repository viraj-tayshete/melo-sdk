/**
 * DistanceSensor.cpp
 *
 * HC-SR04 ultrasonic distance sensor implementation.
 *
 * Working principle:
 *   1. Send 10us HIGH pulse on TRIG
 *   2. ECHO pin goes HIGH for duration proportional to distance
 *   3. Distance (cm) = pulse duration (us) / 58.0
 *      (Speed of sound ~343 m/s, round trip)
 *
 * SDK Version: 0.1.0
 */

#include "DistanceSensor.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

MeloDistanceSensor::MeloDistanceSensor(uint8_t triggerPin, uint8_t echoPin)
    : _trigPin(triggerPin), _echoPin(echoPin), _lastReadMs(0)
{
}

// ---------------------------------------------------------------------------
// Initialization
// ---------------------------------------------------------------------------

void MeloDistanceSensor::begin() {
    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(5);  // Settle time after initialization
}

// ---------------------------------------------------------------------------
// Read distance in cm
// ---------------------------------------------------------------------------

float MeloDistanceSensor::readCm() {
    // Send 10-microsecond trigger pulse
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    // Wait for echo pulse and measure duration
    long duration = pulseIn(_echoPin, HIGH, ULTRASONIC_TIMEOUT_US);

    if (duration == 0) {
        // pulseIn() timed out -- no echo received
        _lastReadMs = millis();
        return -1.0f;
    }

    float distanceCm = _pulseToDistanceCm(duration);

    // Validate range
    if (distanceCm > ULTRASONIC_MAX_CM || distanceCm < 2.0f) {
        _lastReadMs = millis();
        return -1.0f;
    }

    _lastReadMs = millis();
    return distanceCm;
}

// ---------------------------------------------------------------------------
// Read with full result struct
// ---------------------------------------------------------------------------

MeloDistanceReading MeloDistanceSensor::read() {
    MeloDistanceReading result;
    result.distanceCm = readCm();
    result.valid      = (result.distanceCm > 0.0f);
    result.status     = result.valid ? MELO_OK : MELO_ERR_SENSOR;
    return result;
}

// ---------------------------------------------------------------------------
// Proximity check
// ---------------------------------------------------------------------------

bool MeloDistanceSensor::isCloserThan(float thresholdCm) {
    float d = readCm();
    if (d < 0) return false;  // Invalid reading -- assume not close
    return (d < thresholdCm);
}

// ---------------------------------------------------------------------------
// Private: Convert pulse duration to distance
// ---------------------------------------------------------------------------

float MeloDistanceSensor::_pulseToDistanceCm(long pulseMicros) {
    // Speed of sound = ~343 m/s = 0.0343 cm/us
    // Round-trip: divide by 2
    // Distance (cm) = pulseMicros * 0.034300 / 2 = pulseMicros / 58.2
    return (float)pulseMicros / 58.2f;
}
