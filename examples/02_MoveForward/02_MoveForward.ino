/**
 * 02_MoveForward
 *
 * Drive the Melo robot forward, then stop.
 * This is the fundamental Vertical Slice 1 — proves the full
 * SDK -> motor driver -> hardware path works end-to-end.
 *
 * Expected behavior:
 *   - Robot drives forward at 70% speed for 2 seconds
 *   - Robot stops for 1 second
 *   - Repeats
 */

#include <Melo.h>

void setup() {
    Melo.begin();
}

void loop() {
    Melo.moveForward(70);   // Move forward at 70% speed
    delay(2000);            // Drive for 2 seconds

    Melo.stop();            // Stop (active brake)
    delay(1000);            // Wait 1 second
}
