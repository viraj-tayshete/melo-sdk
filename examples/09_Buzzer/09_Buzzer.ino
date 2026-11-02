/**
 * 09_Buzzer
 *
 * Plays a short tune, then a tone that runs while the program does other things.
 */

#include <Melo.h>

void setup() {
    Melo.begin();

    Melo.beep(262, 250);   // C
    Melo.beep(330, 250);   // E
    Melo.beep(392, 250);   // G
    Melo.beep(523, 500);   // high C

    Melo.startBeep(880);   // keeps playing...
    Melo.ledBlink(3, 200); // ...while the LED blinks
    Melo.stopBeep();
}

void loop() {
}
