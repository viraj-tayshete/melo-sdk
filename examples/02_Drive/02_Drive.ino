/**
 * 02_Drive
 *
 * Forward, then backward, then stop. Put the robot on a stand (wheels off the
 * table) the first time you run it.
 *
 * If the robot spins instead of going forward, one wheel is turning the wrong
 * way: flip MELO_RIGHT_WHEEL_INVERTED in MeloConfig.h.
 */

#include <Melo.h>

void setup() {
    Melo.begin();

    Melo.moveForward(70);
    delay(2000);

    Melo.moveBackward(70);
    delay(2000);

    Melo.stop();
}

void loop() {
}
