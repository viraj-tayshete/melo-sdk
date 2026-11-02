/**
 * IRSensor.cpp
 *
 * SDK Version: 0.2.0
 */

#include "IRSensor.h"

MeloIRSensor::MeloIRSensor(uint8_t pin) : _pin(pin) {
}

void MeloIRSensor::begin() {
    pinMode(_pin, INPUT);
}

uint16_t MeloIRSensor::raw() {
#if MELO_IR_ANALOG
    return (uint16_t)analogRead(_pin);
#else
    return (uint16_t)digitalRead(_pin);
#endif
}

bool MeloIRSensor::reflecting() {
#if MELO_IR_ANALOG
    bool low = raw() < MELO_IR_ANALOG_THRESHOLD;
#else
    bool low = (raw() == LOW);
#endif
    return MELO_IR_REFLECT_IS_LOW ? low : !low;
}
