/**
 * MeloBuzzer.h
 *
 * Passive buzzer driver using Arduino tone() (ESP32 core 3.x, LEDC-backed).
 *
 * SDK Version: 0.2.0
 */

#ifndef MELO_BUZZER_H
#define MELO_BUZZER_H

#include <Arduino.h>
#include "../MeloConfig.h"
#include "../MeloTypes.h"

class MeloBuzzerDriver {
public:
    explicit MeloBuzzerDriver(uint8_t pin);

    /** Configure the pin; the buzzer starts silent. */
    void begin();

    /** Play a tone and wait for it to finish (blocking). 0 Hz = silence for the duration. */
    void beep(uint16_t frequencyHz, uint32_t durationMs);

    /** Start a continuous tone (non-blocking). */
    void startTone(uint16_t frequencyHz);

    void stopTone();

    /** Two rising beeps (blocking). */
    void playStartupTone();

    /** Three falling beeps (blocking). */
    void playErrorTone();

private:
    uint8_t _pin;
};

#endif // MELO_BUZZER_H
