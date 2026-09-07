/**
 * 07_DistanceSensor
 *
 * Read the ultrasonic distance sensor and print results to Serial Monitor.
 * Also demonstrates obstacle avoidance: stop when an obstacle is detected.
 *
 * Expected behavior:
 *   - Move forward at 60% speed
 *   - Continuously read distance sensor
 *   - If obstacle closer than 20cm: stop, beep, wait, then resume
 *   - Distance readings printed to Serial Monitor
 */

#include <Melo.h>

const float STOP_DISTANCE_CM = 20.0f;  // Stop if obstacle closer than this

void setup() {
    Melo.begin();
    Serial.println("Distance sensor demo starting...");
    Serial.println("Will stop if obstacle < " + String(STOP_DISTANCE_CM) + " cm");
}

void loop() {
    float distance = Melo.readDistance();

    // Print reading to Serial Monitor
    if (distance < 0) {
        Serial.println("Distance: OUT OF RANGE or ERROR");
    } else {
        Serial.println("Distance: " + String(distance, 1) + " cm");
    }

    // Obstacle detection
    if (Melo.obstacleDetected(STOP_DISTANCE_CM)) {
        Serial.println(">>> OBSTACLE DETECTED! Stopping.");
        Melo.stop();
        Melo.beep(1000, 200);  // Alert beep
        delay(1000);
    } else {
        Melo.moveForward(60);
    }

    delay(100);  // ~10 Hz sensor polling
}
