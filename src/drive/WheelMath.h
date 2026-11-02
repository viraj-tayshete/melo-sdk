/**
 * WheelMath.h
 *
 * Pure speed-to-pulse maths for the continuous-rotation wheel servos.
 * No Arduino dependencies, so it is unit tested on the host (tests/).
 *
 * SDK Version: 0.2.0
 */

#ifndef MELO_WHEEL_MATH_H
#define MELO_WHEEL_MATH_H

#include <stdint.h>
#include "../MeloConfig.h"

/**
 * Pulse width in microseconds for a wheel speed.
 *
 * @param speed     -100..100, positive = robot moves forward
 * @param inverted  true if this servo is mounted mirrored
 * @param trimUs    centre correction in servo space (not mirrored)
 */
inline uint16_t meloWheelPulseUs(int speed, bool inverted, int trimUs) {
    if (speed > 100)  speed = 100;
    if (speed < -100) speed = -100;
    if (inverted) speed = -speed;

    long pulse = (long)MELO_SERVO_CENTER_US + trimUs
               + (long)speed * MELO_SERVO_SPAN_US / 100;
    if (pulse < MELO_SERVO_MIN_US) pulse = MELO_SERVO_MIN_US;
    if (pulse > MELO_SERVO_MAX_US) pulse = MELO_SERVO_MAX_US;
    return (uint16_t)pulse;
}

/** LEDC duty value for a pulse width at MELO_SERVO_FREQ_HZ and MELO_SERVO_RES_BITS. */
inline uint32_t meloPulseToDuty(uint16_t pulseUs) {
    return ((uint32_t)pulseUs << MELO_SERVO_RES_BITS) / MELO_SERVO_PERIOD_US;
}

#endif // MELO_WHEEL_MATH_H
