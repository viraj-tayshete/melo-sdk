/**
 * MeloWheel.h
 *
 * One wheel driven by a continuous-rotation servo (ESP32 LEDC, core 3.x).
 *
 * SDK Version: 0.2.0
 */

#ifndef MELO_WHEEL_H
#define MELO_WHEEL_H

#include <Arduino.h>
#include "../MeloConfig.h"
#include "../MeloTypes.h"
#include "WheelMath.h"

#if !defined(ESP32)
  #error "Melo needs an ESP32-S3 board. Choose Tools > Board > esp32 > ESP32S3 Dev Module."
#else
  #include <esp_arduino_version.h>
  #if ESP_ARDUINO_VERSION_MAJOR < 3
    #error "Melo needs the ESP32 Arduino core 3.x. In Boards Manager install 'esp32 by Espressif Systems' (3.x); the 'Arduino ESP32 Boards' package is based on core 2.x and is not supported yet."
  #endif
#endif

class MeloWheelDriver {
public:
    /**
     * @param pin       servo signal pin
     * @param inverted  true if this servo is mounted mirrored
     * @param trimUs    centre correction in microseconds
     */
    MeloWheelDriver(uint8_t pin, bool inverted, int16_t trimUs);

    /** Attach the PWM output. Returns false if the pin could not be set up. */
    bool begin();

    /** Set speed -100..100 (positive = robot forward). 0 stops the wheel. */
    void setSpeed(int8_t speed);

    /** Stop the wheel (see MELO_STOP_CUTS_PULSE). */
    void stop();

    /** Change the centre correction; takes effect on the next setSpeed()/stop(). */
    void setTrimUs(int16_t trimUs) { _trimUs = trimUs; }

private:
    uint8_t _pin;
    bool    _inverted;
    int16_t _trimUs;
    bool    _attached;

    void _writePulse(uint16_t pulseUs);
};

#endif // MELO_WHEEL_H
