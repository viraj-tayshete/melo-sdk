/**
 * 06_ObstacleAvoidance
 *
 * Drives forward. When the IR sensor sees something in front, the robot
 * beeps, backs up, turns, and carries on.
 *
 * IR only says "something is there", not how far away it is. Adjust the
 * detection range with the small screw on the sensor module.
 */

#include <Melo.h>

void setup() {
    Melo.begin();
    delay(2000);
}

void loop() {
    if (Melo.obstacleDetected()) {
        Melo.stop();
        Melo.beep(1200, 150);

        Melo.moveBackward(50);
        delay(600);

        Melo.turnRight(55);
        delay(500);
        Melo.stop();
    } else {
        Melo.moveForward(60);
    }
    delay(20);
}
