/**
 * PortMath.h
 *
 * Estimates the ID resistor of a plug-in block from the voltage on its port's
 * Add line, which forms a divider with the board's 10k pull-up to 3.3V:
 *
 *     V = 3.3V * R / (R_pullup + R)   =>   R = R_pullup * raw / (adcMax - raw)
 *
 * Pure maths, no Arduino dependencies (unit tested on the host). The ESP32 ADC
 * is not linear near either end, so treat the result as an estimate.
 *
 * SDK Version: 0.2.0
 */

#ifndef MELO_PORT_MATH_H
#define MELO_PORT_MATH_H

#include <stdint.h>
#include "../MeloConfig.h"

/** Readings at or above this fraction of full scale mean "nothing plugged in" (pulled up). */
#define MELO_PORT_OPEN_RAW  (MELO_ADC_MAX - 20)

/**
 * @return the estimated resistance to ground in ohms, 0 if the line is shorted
 *         to ground, or -1 if nothing is plugged in (the pull-up wins).
 */
inline int32_t meloPortOhms(uint16_t raw) {
    if (raw >= MELO_PORT_OPEN_RAW) return -1;
    if (raw == 0) return 0;
    return (int32_t)((uint64_t)MELO_PORT_PULLUP_OHMS * raw / (MELO_ADC_MAX - raw));
}

#endif // MELO_PORT_MATH_H
