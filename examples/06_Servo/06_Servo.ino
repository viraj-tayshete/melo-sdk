/**
 * 06_Servo
 *
 * Control the Melo servo.
 *
 * Expected behavior:
 *   - Sweep servo from 0 to 180 degrees in steps
 *   - Return to center (90 degrees)
 *   - Repeat
 */

#include <Melo.h>

void setup() {
    Melo.begin();
    Melo.servoCenter(1);  // Start at center position
    delay(500);
}

void loop() {
    // Sweep from 0 to 180 degrees
    Serial.println("Sweeping 0 -> 180...");
    for (int angle = 0; angle <= 180; angle += 10) {
        Melo.servo(1, angle);
        delay(100);
    }

    delay(500);

    // Sweep back from 180 to 0 degrees
    Serial.println("Sweeping 180 -> 0...");
    for (int angle = 180; angle >= 0; angle -= 10) {
        Melo.servo(1, angle);
        delay(100);
    }

    delay(500);

    // Return to center
    Serial.println("Centering...");
    Melo.servoCenter(1);
    delay(1000);
}
