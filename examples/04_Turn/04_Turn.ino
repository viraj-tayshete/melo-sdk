/**
 * 04_Turn
 *
 * Demonstrate turning left and right.
 *
 * Expected behavior:
 *   - Forward for 1 second
 *   - Turn left for 0.5 seconds
 *   - Forward for 1 second
 *   - Turn right for 0.5 seconds
 *   - Stop for 1 second
 *   - Repeat
 */

#include <Melo.h>

void setup() {
    Melo.begin();
}

void loop() {
    Melo.moveForward(60);
    delay(1000);

    Melo.turnLeft(50);    // Turn left at 50% speed
    delay(500);

    Melo.moveForward(60);
    delay(1000);

    Melo.turnRight(50);   // Turn right at 50% speed
    delay(500);

    Melo.stop();
    delay(1000);
}
