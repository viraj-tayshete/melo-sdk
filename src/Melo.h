/**
 * Melo.h
 *
 * Melo SDK - Public API
 *
 * The single header users (and the Melo Blocks code generator) include.
 *
 *   #include <Melo.h>
 *
 *   void setup() { Melo.begin(); }
 *   void loop() {
 *       Melo.moveForward(70);
 *       delay(1000);
 *       Melo.stop();
 *       delay(1000);
 *   }
 *
 * Hardware: Arduino Nano ESP32, two continuous-rotation servo wheels, one IR
 * sensor, a light sensor, one LED and a buzzer.
 *
 * Every public method that is a robot capability must be listed in
 * docs/CAPABILITY_MODEL.json; tools/check_capabilities.py enforces this.
 *
 * SDK Version: 0.2.0
 */

#ifndef MELO_H
#define MELO_H

#include <Arduino.h>
#include "MeloConfig.h"
#include "MeloTypes.h"
#include "drive/MeloWheel.h"
#include "sensors/IRSensor.h"
#include "leds/MeloLED.h"
#include "buzzer/MeloBuzzer.h"
#include "behaviors/LineFollowLogic.h"
#include "ports/PortMath.h"

class MeloClass {
public:
    MeloClass();

    // -----------------------------------------------------------------------
    // SYSTEM
    // -----------------------------------------------------------------------

    /**
     * Set up the robot. Call once at the start of setup().
     * Plays a short startup sound and blinks the LED.
     *
     * @return MELO_OK, or MELO_ERR_PIN if a wheel pin could not be set up.
     */
    MeloStatus begin();

    bool isReady() const { return _initialized; }

    String getSdkVersion() const;
    MeloDeviceInfo getDeviceInfo() const;

    MeloStatus lastError() const { return _lastError; }
    String lastErrorMessage() const;
    String statusMessage(MeloStatus status) const;

    /** Print version, board and last error to Serial. */
    void printStatus() const;

    // -----------------------------------------------------------------------
    // DRIVING
    // -----------------------------------------------------------------------

    /** Drive forward. speed 0-100 (percent), values above 100 are clamped. */
    void moveForward(uint8_t speed = MELO_MOVE_DEFAULT_SPEED_PCT);

    /** Drive backward. speed 0-100 (percent). */
    void moveBackward(uint8_t speed = MELO_MOVE_DEFAULT_SPEED_PCT);

    /**
     * Spin left on the spot until stopped. speed 0-100 (percent of wheel
     * speed). This is a speed, not an angle.
     */
    void turnLeft(uint8_t speed = MELO_TURN_DEFAULT_SPEED_PCT);

    /** Spin right on the spot until stopped. speed 0-100 (percent). */
    void turnRight(uint8_t speed = MELO_TURN_DEFAULT_SPEED_PCT);

    /** Stop both wheels. */
    void stop();

    /**
     * Set one wheel (or both) directly.
     * @param side   WHEEL_LEFT, WHEEL_RIGHT or WHEEL_BOTH
     * @param speed  -100..100 (positive = forward, negative = backward, 0 = stop)
     */
    void wheel(MeloWheelSide side, int8_t speed);

    /**
     * Correct a servo that creeps when it should be stopped.
     * @param side      WHEEL_LEFT, WHEEL_RIGHT or WHEEL_BOTH
     * @param trimUs    microseconds added to the centre pulse (-200..200)
     */
    void trimWheel(MeloWheelSide side, int16_t trimUs);

    /**
     * Follow a line using the IR sensor. Call it repeatedly:
     *
     *     while (Melo.followLine(50)) { }
     *
     * Each call does one short control step (about 5 ms). It returns false
     * once the line was lost and could not be found again; the robot has then
     * stopped and played an error sound. A pause of more than half a second
     * between calls starts a fresh run.
     *
     * Assumes a dark line on a light floor, and the sensor facing down in
     * front of the wheels. Timings are untuned (see MeloConfig.h).
     *
     * @param speed  cruising speed 0-100 (percent)
     */
    bool followLine(uint8_t speed = MELO_LINE_DEFAULT_SPEED_PCT);

    /** Forget a lost line and start following from scratch. */
    void resetLineFollow();

    /** End the program: stop everything and wait forever. */
    void halt();

    // -----------------------------------------------------------------------
    // IR SENSOR (one sensor, three meanings)
    // -----------------------------------------------------------------------

    /** True when the IR sensor sees a reflection. */
    bool irDetected();

    /** True when the sensor is over a dark line (no reflection). */
    bool lineDetected();

    /** True when something reflects IR in front of the sensor. Not a distance. */
    bool obstacleDetected();

    /** Raw sensor value: 0/1 (digital module) or 0-4095 (analog module). */
    uint16_t irRaw();

    // -----------------------------------------------------------------------
    // PORTS (diagnostics for the plug-in blocks)
    // -----------------------------------------------------------------------

    /**
     * Raw 12-bit reading (0-4095) of a port's Add line. With nothing plugged
     * in the 10k pull-up holds it near 4095. Returns 0 and sets
     * MELO_ERR_INVALID_ARG for a port outside 1-6.
     */
    uint16_t portRaw(uint8_t port);

    /**
     * Estimated resistance of the ID resistor in the block plugged into a
     * port, in ohms: 0 = shorted to ground, -1 = nothing plugged in or the
     * port is invalid. An estimate only (the ESP32 ADC is not linear).
     */
    int32_t portOhms(uint8_t port);

    // -----------------------------------------------------------------------
    // LIGHT SENSOR
    // -----------------------------------------------------------------------

    /** Light level as a percentage, 0 (dark) to 100 (bright). */
    uint8_t readLight();

    /** Raw 12-bit reading, 0-4095. */
    uint16_t readLightRaw();

    // -----------------------------------------------------------------------
    // LED
    // -----------------------------------------------------------------------

    /** Turn the LED on or off. */
    void led(bool on);

    /** Blink the LED (blocking). offMs defaults to onMs. */
    void ledBlink(uint8_t times, uint16_t onMs, uint16_t offMs = 0);

    // -----------------------------------------------------------------------
    // BUZZER
    // -----------------------------------------------------------------------

    /** Play a tone and wait for it to finish (blocking). 20-20000 Hz, 0 = silence. */
    void beep(uint16_t frequencyHz, uint32_t durationMs);

    /** Start a continuous tone (non-blocking). */
    void startBeep(uint16_t frequencyHz);

    /** Stop the tone. */
    void stopBeep();

private:
    bool        _initialized;
    MeloStatus  _lastError;

    MeloWheelDriver  _leftWheel;
    MeloWheelDriver  _rightWheel;
    MeloIRSensor     _ir;
    MeloLEDDriver    _led;
    MeloBuzzerDriver _buzzer;

    MeloLineFollowLogic _lineLogic;
    uint32_t            _lastFollowMs;

    void _setError(MeloStatus err);
    void _debugPrint(const String& msg) const;
    bool _checkInit(const char* callerName);
};

extern MeloClass Melo;

#endif // MELO_H
