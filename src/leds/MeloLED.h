/**
 * MeloLED.h
 *
 * LED driver for the Melo robotics platform.
 * Initially supports simple digital on/off LEDs.
 *
 * ASSUMPTION: Simple digital GPIO LEDs (not RGB/NeoPixel).
 * If RGB LEDs are confirmed on the PCB, extend this class with
 * setColor(r, g, b) and use FastLED or Adafruit NeoPixel library.
 *
 * SDK Version: 0.1.0
 */

#ifndef MELO_LED_H
#define MELO_LED_H

#include <Arduino.h>
#include "../MeloConfig.h"
#include "../MeloTypes.h"

class MeloLEDDriver {
public:
    /**
     * @param pin       GPIO pin connected to LED anode (active HIGH assumed)
     * @param activeHigh  true = HIGH turns LED on; false = LOW turns LED on
     */
    MeloLEDDriver(uint8_t pin, bool activeHigh = true);

    /** Configure GPIO. Must be called before use. */
    void begin();

    /**
     * Set LED state.
     *
     * @param on  true = LED on, false = LED off
     */
    void set(bool on);

    /** Turn LED on. */
    void on();

    /** Turn LED off. */
    void off();

    /** Toggle LED state. */
    void toggle();

    /**
     * Blink LED.
     *
     * @param times      Number of blinks
     * @param onMs       On duration in milliseconds
     * @param offMs      Off duration in milliseconds (defaults to onMs)
     *
     * NOTE: Blocking — will delay sketch execution.
     */
    void blink(uint8_t times, uint16_t onMs, uint16_t offMs = 0);

    /** Returns current LED state. */
    bool isOn() const { return _isOn; }

private:
    uint8_t _pin;
    bool    _activeHigh;
    bool    _isOn;

    void _write(bool state);
};

#endif // MELO_LED_H
