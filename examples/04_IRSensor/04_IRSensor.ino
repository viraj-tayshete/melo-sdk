/**
 * 04_IRSensor
 *
 * One IR sensor, three meanings. Hold your hand in front of the sensor, then
 * move it over a dark line, and watch the Serial Monitor.
 */

#include <Melo.h>

void setup() {
    Melo.begin();
}

void loop() {
    Serial.print("reflection: ");
    Serial.print(Melo.irDetected() ? "yes" : "no ");
    Serial.print("   on line: ");
    Serial.print(Melo.lineDetected() ? "yes" : "no ");
    Serial.print("   obstacle: ");
    Serial.print(Melo.obstacleDetected() ? "yes" : "no ");
    Serial.print("   raw: ");
    Serial.println(Melo.irRaw());
    delay(200);
}
