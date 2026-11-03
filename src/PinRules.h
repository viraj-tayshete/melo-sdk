/**
 * PinRules.h
 *
 * Which GPIOs of the ESP32-S3-WROOM-1 module are safe to use for Melo parts.
 * Pure logic, no Arduino dependencies (unit tested on the host).
 *
 * Reserved, from Espressif's ESP32-S3 / WROOM-1 documentation (written from
 * memory: confirm against the datasheet of the exact module variant):
 *   0, 3, 45, 46   strapping pins (they set boot mode and flash voltage)
 *   19, 20         USB D-/D+ (needed for USB upload and USB serial)
 *   22-25          do not exist on the ESP32-S3
 *   26-37          flash and PSRAM; 33-37 are taken by octal PSRAM/flash variants
 *   43, 44         UART0 TX/RX (default console and upload)
 *   49 and above   do not exist
 *
 * To use a reserved pin on purpose, define MELO_ALLOW_RESERVED_PINS=1 for the build.
 *
 * SDK Version: 0.2.0
 */

#ifndef MELO_PIN_RULES_H
#define MELO_PIN_RULES_H

constexpr bool meloGpioUsable(int pin) {
    if (pin < 0 || pin > 48) return false;
    if (pin == 0 || pin == 3 || pin == 45 || pin == 46) return false;
    if (pin == 19 || pin == 20) return false;
    if (pin >= 22 && pin <= 37) return false;
    if (pin == 43 || pin == 44) return false;
    return true;
}

#endif // MELO_PIN_RULES_H
