/**
 * IRSensor.h
 *
 * One IR reflective sensor. The SDK exposes it in three roles
 * (reflection, line, obstacle); this driver only knows "reflecting or not".
 *
 * SDK Version: 0.2.0
 */

#ifndef MELO_IR_SENSOR_H
#define MELO_IR_SENSOR_H

#include <Arduino.h>
#include "../MeloConfig.h"
#include "../MeloTypes.h"

class MeloIRSensor {
public:
    explicit MeloIRSensor(uint8_t pin);

    void begin();

    /** True when the sensor sees IR reflected back (a surface or object is in range). */
    bool reflecting();

    /** Raw reading: 0/1 for a digital module, 0..4095 for an analog one. */
    uint16_t raw();

private:
    uint8_t _pin;
};

#endif // MELO_IR_SENSOR_H
