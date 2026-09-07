/**
 * MeloBuzzer.h
 *
 * Passive buzzer driver for the Melo robotics platform.
 *
 * Uses Arduino tone() function (ESP8266) or ESP32 LEDC PWM.
 * ASSUMPTION: Passive buzzer on a single GPIO pin.
 *
 * SDK Version: 0.1.0
 */

#ifndef MELO_BUZZER_H
#define MELO_BUZZER_H

#include <Arduino.h>
#include "../MeloConfig.h"
#include "../MeloTypes.h"

class MeloBuzzerDriver {
public:
    /**
     * @param pin     GPIO pin connected to buzzer
     * @param pwmCh   ESP32 LEDC channel (ignored on ESP8266/AVR)
     */
    MeloBuzzerDriver(uint8_t pin, uint8_t pwmCh = BUZZER_PWM_CH);

    /** Configure GPIO. Must be called before use. */
    void begin();

    /**
     * Play a tone for a given duration (blocking).
     *
     * @param frequencyHz  Frequency in Hz (20-20000). 0 = silence.
     * @param durationMs   Duration in milliseconds.
     */
    void beep(uint16_t frequencyHz, uint32_t durationMs);

    /**
     * Start playing a tone continuously (non-blocking).
     * Call stop() to end the tone.
     *
     * @param frequencyHz  Frequency in Hz
     */
    void startTone(uint16_t frequencyHz);

    /** Stop any currently playing tone. */
    void stopTone();

    /**
     * Play a simple startup melody (blocking).
     * Useful for indicating successful initialization.
     */
    void playStartupTone();

    /**
     * Play an error tone (blocking).
     * Useful for indicating error conditions.
     */
    void playErrorTone();

    /** Returns true if tone is currently active. */
    bool isPlaying() const { return _isPlaying; }

private:
    uint8_t  _pin;
    uint8_t  _pwmCh;
    bool     _isPlaying;

    void _startPWM(uint16_t frequencyHz);
    void _stopPWM();
};

#endif // MELO_BUZZER_H
