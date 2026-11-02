/**
 * 08_LED
 *
 * LED on, off, and blinking.
 */

#include <Melo.h>

void setup() {
    Melo.begin();
}

void loop() {
    Melo.led(true);
    delay(500);
    Melo.led(false);
    delay(500);

    Melo.ledBlink(3, 100);   // 3 quick blinks
    delay(1000);
}
