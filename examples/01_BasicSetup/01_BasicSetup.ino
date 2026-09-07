/**
 * 01_BasicSetup
 *
 * The most basic Melo sketch.
 * Connects to the Melo hardware, verifies initialization,
 * and prints device info to Serial Monitor.
 *
 * Steps to run:
 *   1. Install the Melo library in Arduino IDE
 *   2. Select your ESP32 board in Tools > Board
 *   3. Select the correct COM port
 *   4. Upload this sketch
 *   5. Open Serial Monitor at 115200 baud
 *   6. You should hear a startup beep and see device info
 */

#include <Melo.h>

void setup() {
    // Initialize Melo. This must be called first, before any other Melo API.
    MeloStatus status = Melo.begin();

    if (status != MELO_OK) {
        // If begin() fails, print the error and halt
        Serial.println("Melo initialization failed: " + Melo.lastErrorMessage());
        while (true) {
            delay(1000);  // Halt
        }
    }

    // Print device info to Serial Monitor
    Melo.printStatus();

    Serial.println("SDK Version:  " + Melo.getSdkVersion());
    Serial.println("Setup complete. Robot is ready!");
}

void loop() {
    // Nothing to do in this example.
    // Open Serial Monitor and verify the startup output.
    delay(5000);
    Serial.println("Melo is running. isReady = " + String(Melo.isReady() ? "YES" : "NO"));
}
