/**
 * 09_LED
 *
 * Control the Melo LED.
 *
 * Expected behavior:
 *   - LED on for 500ms
 *   - LED off for 500ms
 *   - 3 rapid blinks
 *   - Repeat
 */

#include <Melo.h>

void setup() {
    Melo.begin();
}

void loop() {
    // Simple on/off
    Melo.led(1, true);    // LED on
    delay(500);
    Melo.led(1, false);   // LED off
    delay(500);

    // 3 rapid blinks
    Melo.ledBlink(1, 3, 100, 100);  // 3 blinks, 100ms on, 100ms off
    delay(1000);
}
