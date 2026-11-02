/**
 * 05_LineFollow
 *
 * Follows a dark line on a light floor. Put the robot on the line, with the
 * IR sensor pointing at the floor in front of the wheels.
 *
 * followLine() returns false when the line is lost for good; the robot stops
 * and plays a sad sound.
 */

#include <Melo.h>

void setup() {
    Melo.begin();

    Melo.led(true);
    delay(3000);            // time to put the robot down
    Melo.led(false);

    while (Melo.followLine(50)) {
    }
    Melo.halt();
}

void loop() {
}
