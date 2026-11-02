/**
 * 03_Turn
 *
 * Spins left, then right. The number is wheel speed, not degrees: how far the
 * robot turns depends on how long you wait.
 */

#include <Melo.h>

void setup() {
    Melo.begin();

    Melo.turnLeft(60);
    delay(800);
    Melo.stop();
    delay(500);

    Melo.turnRight(60);
    delay(800);
    Melo.stop();
}

void loop() {
}
