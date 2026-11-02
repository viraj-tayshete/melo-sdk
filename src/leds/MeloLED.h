/**
 * MeloLED.h
 *
 * Simple on/off LED driver.
 *
 * SDK Version: 0.2.0
 */

#ifndef MELO_LED_H
#define MELO_LED_H

#include <Arduino.h>
#include "../MeloConfig.h"
#include "../MeloTypes.h"

class MeloLEDDriver {
public:
    /**
     * @param pin         GPIO pin connected to the LED
     * @param activeHigh  true = HIGH turns the LED on; false = LOW turns it on
     */
    MeloLEDDriver(uint8_t pin, bool activeHigh = true);

    /** Configure the pin; the LED starts off. */
    void begin();

    void set(bool on);

    /** Blink the LED (blocking). offMs defaults to onMs. */
    void blink(uint8_t times, uint16_t onMs, uint16_t offMs = 0);

    bool isOn() const { return _isOn; }

private:
    uint8_t _pin;
    bool    _activeHigh;
    bool    _isOn;
};

#endif // MELO_LED_H
