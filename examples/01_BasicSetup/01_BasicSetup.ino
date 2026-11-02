/**
 * 01_BasicSetup
 *
 * Starts the robot and prints its details to the Serial Monitor (115200 baud).
 * You should hear two beeps and see the LED blink twice.
 */

#include <Melo.h>

void setup() {
    if (Melo.begin() != MELO_OK) {
        Serial.println(Melo.lastErrorMessage());
        Melo.halt();
    }
    Melo.printStatus();
}

void loop() {
}
