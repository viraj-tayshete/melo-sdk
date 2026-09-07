/**
 * DistanceSensor.h
 *
 * Ultrasonic distance sensor driver for the Melo platform.
 * Assumes HC-SR04 (or compatible) using Trigger + Echo GPIO pins.
 *
 * ASSUMPTION: HC-SR04 sensor. Verify sensor model and pin connections
 * against the Melo PCB schematic before use.
 *
 * SDK Version: 0.1.0
 */

#ifndef MELO_DISTANCE_SENSOR_H
#define MELO_DISTANCE_SENSOR_H

#include <Arduino.h>
#include "../MeloConfig.h"
#include "../MeloTypes.h"

class MeloDistanceSensor {
public:
    /**
     * @param triggerPin  GPIO pin connected to HC-SR04 TRIG
     * @param echoPin     GPIO pin connected to HC-SR04 ECHO
     */
    MeloDistanceSensor(uint8_t triggerPin, uint8_t echoPin);

    /** Configure GPIO pins. Must be called before reading. */
    void begin();

    /**
     * Read distance in centimeters.
     *
     * @return Distance in cm. Returns -1.0 if out of range or error.
     *
     * Blocking: Takes up to ~30ms per call.
     * Minimum recommended interval between calls: 50ms.
     */
    float readCm();

    /**
     * Read distance and return a full result struct with validity and status.
     */
    MeloDistanceReading read();

    /**
     * Check if an object is closer than a given threshold.
     *
     * @param thresholdCm  Distance threshold in centimeters
     * @return true if object is within threshold distance
     */
    bool isCloserThan(float thresholdCm);

private:
    uint8_t _trigPin;
    uint8_t _echoPin;
    unsigned long _lastReadMs;

    float _pulseToDistanceCm(long pulseMicros);
};

#endif // MELO_DISTANCE_SENSOR_H
