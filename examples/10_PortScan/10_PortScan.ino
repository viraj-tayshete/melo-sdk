/**
 * 10_PortScan
 *
 * Shows what is plugged into each of the six ports (J1-J6). Each block has an
 * ID resistor from the port's Add line to ground; the board pulls the line up
 * to 3.3V with 10k, so the voltage reveals the resistor.
 *
 * Use it to measure the real value of each block's resistor: plug one block
 * in at a time and write down the "ohms" column. "-" means nothing is plugged
 * in. Open the Serial Monitor at 115200 baud.
 *
 * The result is only meaningful once the port pins in MeloConfig.h match the PCB.
 */

#include <Melo.h>

void setup() {
    Melo.begin();
}

void loop() {
    Serial.println("port   raw    ohms");
    for (uint8_t port = 1; port <= 6; port++) {
        int32_t ohms = Melo.portOhms(port);
        Serial.print("  ");
        Serial.print(port);
        Serial.print("    ");
        Serial.print(Melo.portRaw(port));
        Serial.print("    ");
        if (ohms < 0) {
            Serial.println("-");
        } else {
            Serial.println(ohms);
        }
    }
    Serial.println();
    delay(1000);
}
