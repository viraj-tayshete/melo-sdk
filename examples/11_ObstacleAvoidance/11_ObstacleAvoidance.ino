/**
 * 11_ObstacleAvoidance
 *
 * Autonomous obstacle avoidance using distance sensor.
 *
 * Behavior:
 *   - Drive forward
 *   - If obstacle detected within 20cm:
 *     - Stop
 *     - Beep and blink LED
 *     - Back up briefly
 *     - Turn right
 *     - Resume forward
 */

#include <Melo.h>

const float OBSTACLE_DISTANCE_CM = 20.0f;
const uint8_t DRIVE_SPEED   = 60;
const uint8_t REVERSE_SPEED = 50;
const uint8_t TURN_SPEED    = 55;

void setup() {
    Melo.begin();
    Serial.println("Obstacle avoidance demo. Press reset to stop.");
    delay(2000);  // 2-second delay before starting
}

void loop() {
    float distance = Melo.readDistance();

    if (distance > 0 && distance < OBSTACLE_DISTANCE_CM) {
        // Obstacle detected!
        Serial.println("Obstacle at " + String(distance, 1) + "cm — avoiding...");

        Melo.stop();
        Melo.beep(1200, 150);
        Melo.led(1, true);

        // Back up
        Melo.moveBackward(REVERSE_SPEED);
        delay(600);

        Melo.stop();
        delay(200);

        // Turn right
        Melo.turnRight(TURN_SPEED);
        delay(500);

        Melo.led(1, false);
        Melo.stop();
        delay(200);

    } else {
        // Path clear — drive forward
        Melo.moveForward(DRIVE_SPEED);
    }

    delay(50);  // ~20Hz main loop
}
