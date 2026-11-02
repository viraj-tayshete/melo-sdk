/**
 * Compile-only test: on the Arduino Nano ESP32 the SDK must use the board's
 * Arduino pin names, not the generic ESP32 fallback numbers. Never uploaded.
 */

#include <Melo.h>

#if defined(ARDUINO_NANO_ESP32)
static_assert(MELO_LEFT_WHEEL_PIN == D8,  "left wheel pin should be D8 on the Nano ESP32");
static_assert(MELO_RIGHT_WHEEL_PIN == D9, "right wheel pin should be D9 on the Nano ESP32");
static_assert(MELO_PORT1_SIGNAL_PIN == D2 && MELO_PORT6_SIGNAL_PIN == D7, "port signal pins should be D2..D7");
static_assert(MELO_PORT1_ADD_PIN == A0 && MELO_PORT6_ADD_PIN == A5,       "port Add pins should be A0..A5");
// parts follow their ports: IR port 1, light port 2, LED port 3, buzzer port 4
static_assert(MELO_IR_PIN == D2 && MELO_LIGHT_PIN == D3 && MELO_LED_PIN == D4 && MELO_BUZZER_PIN == D5,
              "parts should follow their port assignments");
#endif

void setup() {
}

void loop() {
}
