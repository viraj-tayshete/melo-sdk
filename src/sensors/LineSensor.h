/**
 * LineSensor.h
 *
 * Line sensor driver for the Melo platform.
 * Assumes a TCRT5000 IR reflective sensor (or similar).
 * Can operate in digital mode (threshold comparison) or analog mode (raw ADC).
 *
 * ASSUMPTION: TCRT5000 single sensor (digital output).
 * Verify sensor model, output type, and pin connections before use.
 *
 * SDK Version: 0.1.0
 */

#ifndef MELO_LINE_SENSOR_H
#define MELO_LINE_SENSOR_H

#include <Arduino.h>
#include "../MeloConfig.h"
#include "../MeloTypes.h"

class MeloLineSensor {
public:
    /**
     * @param pin         GPIO pin connected to sensor output
     * @param analogMode  true = use analogRead(); false = use digitalRead()
     */
    MeloLineSensor(uint8_t pin, bool analogMode = LINE_SENSOR_ANALOG);

    /** Configure GPIO. Must be called before reading. */
    void begin();

    /**
     * Read whether a line is detected.
     *
     * In digital mode: returns true if sensor output is LOW (dark surface/line).
     * In analog mode:  returns true if ADC value is below threshold (dark surface).
     *
     * @return true = line detected (dark surface)
     */
    bool onLine();

    /**
     * Read raw sensor value.
     *
     * Digital mode: returns 0 or 1
     * Analog mode:  returns 0-4095 (ESP32 12-bit ADC)
     */
    uint16_t rawValue();

    /**
     * Full reading struct.
     */
    MeloLineSensorReading read();

    /**
     * Set analog threshold for line detection (analog mode only).
     * Values below threshold = line detected.
     *
     * @param threshold  ADC threshold value (0-4095)
     */
    void setThreshold(uint16_t threshold);

private:
    uint8_t  _pin;
    bool     _analogMode;
    uint16_t _threshold;
};

#endif // MELO_LINE_SENSOR_H
