/**
 * Compile-only test: the default pins must be the Melo Mini placeholders on
 * the ESP32-S3, and parts must follow their port assignments. Never uploaded.
 */

#include <Melo.h>
#include <PinRules.h>

static_assert(MELO_LEFT_WHEEL_PIN == 14 && MELO_RIGHT_WHEEL_PIN == 15, "wheel pins should be GPIO14 and GPIO15");
static_assert(MELO_PORT1_SIGNAL_PIN == 8 && MELO_PORT6_SIGNAL_PIN == 13, "port signal pins should be GPIO8..13");
static_assert(MELO_PORT1_ADD_PIN == 1 && MELO_PORT6_ADD_PIN == 7,        "port Add pins should be GPIO1,2,4,5,6,7");
// parts follow their ports: IR port 1, light port 2, LED port 3, buzzer port 4
static_assert(MELO_IR_PIN == 8 && MELO_LIGHT_PIN == 9 && MELO_LED_PIN == 10 && MELO_BUZZER_PIN == 11,
              "parts should follow their port assignments");

void setup() {
}

void loop() {
}
