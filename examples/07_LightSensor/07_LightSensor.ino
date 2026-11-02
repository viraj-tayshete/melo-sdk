/**
 * 07_LightSensor
 *
 * Shows the light level and lights the LED when it gets dark.
 * Cover the sensor with your hand to try it.
 */

#include <Melo.h>

void setup() {
    Melo.begin();
}

void loop() {
    uint8_t light = Melo.readLight();   // 0 = dark, 100 = bright
    Serial.print("light: ");
    Serial.print(light);
    Serial.println("%");

    Melo.led(light < 30);
    delay(200);
}
