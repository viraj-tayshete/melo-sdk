# Melo SDK

> Make Melo hardware feel like a programmable robot, not an ESP development board.

The Melo SDK is the Arduino library behind the Melo robot kit. It is also the library that **Melo Blocks** generates code against: a child arranges blocks, Blocks writes a few lines of Arduino code, and this library does the rest.

```cpp
#include <Melo.h>

void setup() {
    Melo.begin();
    Melo.moveForward(70);
    delay(2000);
    Melo.stop();
}

void loop() {
}
```

## Status

**Version 0.2.0, work in progress. It has not been run on a real robot yet.**

- It compiles for the Arduino Nano ESP32 (ESP32 core 3.3.3); the host tests pass.
- The board has six identical ports (see [the hardware inventory](docs/HARDWARE_CAPABILITY_INVENTORY.md)). Every pin in [src/MeloConfig.h](src/MeloConfig.h), and which part sits in which port, is a **placeholder** until it is checked against the PCB.
- The IR sensor type, the light sensor circuit and the servo wiring are **unconfirmed**.
- It is not yet in the Arduino Library Manager. Install it from a ZIP (below).

## The kit

| Part | Library support |
|------|-----------------|
| Arduino Nano ESP32 | The only supported board |
| 2 x continuous-rotation (360°) SG90 servos, one per wheel | `moveForward`, `moveBackward`, `turnLeft`, `turnRight`, `stop`, `wheel`, `followLine` |
| 1 IR sensor | `irDetected`, `lineDetected`, `obstacleDetected` |
| Light sensor | `readLight` |
| White LED | `led`, `ledBlink` |
| Buzzer | `beep`, `startBeep`, `stopBeep` |

There is no distance sensor, button, IMU or battery sensing, so the library has no functions for them.

## Installation

1. Download this repository as a ZIP.
2. In the Arduino IDE: **Sketch → Include Library → Add .ZIP Library...**
3. In **Boards Manager** install **esp32 by Espressif Systems** (version 3.x), then choose **Tools → Board → esp32 → Arduino Nano ESP32**. The separate *Arduino ESP32 Boards* package is based on core 2.x and is **not supported yet** (the library stops with an error message).
4. Open an example from **File → Examples → Melo** and upload it.

If a wheel spins the wrong way, flip `MELO_RIGHT_WHEEL_INVERTED` (or the left one) in `src/MeloConfig.h`.

## Safety notes

- Run the wheel servos from a power source that can supply their current (not the board's 3.3 V pin), with a common ground. A starting servo can reset the board.
- The Nano ESP32 pins are 3.3 V only and are not 5 V tolerant. Check the voltage of the sensors' outputs.
- Try the first drive example with the wheels off the ground.

## API at a glance

| Group | Functions |
|-------|-----------|
| System | `begin`, `isReady`, `printStatus`, `lastErrorMessage` |
| Ports | `portRaw`, `portOhms` |
| Driving | `moveForward`, `moveBackward`, `turnLeft`, `turnRight`, `stop`, `wheel`, `trimWheel` |
| Line following | `followLine`, `resetLineFollow` |
| IR sensor | `irDetected`, `lineDetected`, `obstacleDetected`, `irRaw` |
| Light | `readLight`, `readLightRaw` |
| LED | `led`, `ledBlink` |
| Buzzer | `beep`, `startBeep`, `stopBeep` |
| Program | `halt` |

Full details: [docs/API_REFERENCE.md](docs/API_REFERENCE.md).

## Examples

`01_BasicSetup`, `02_Drive`, `03_Turn`, `04_IRSensor`, `05_LineFollow`, `06_ObstacleAvoidance`, `07_LightSensor`, `08_LED`, `09_Buzzer`, `10_PortScan` (measure the ID resistor of each block).

## For Melo Blocks

[docs/CAPABILITY_MODEL.json](docs/CAPABILITY_MODEL.json) lists every block (label, inputs, ranges, defaults, and the Arduino code each one generates). It is the single source of truth: the SDK and Blocks must expose the same capabilities.

## Development

```sh
tests/run.sh                                     # host unit tests (any C++17 compiler)
python3 tools/check_capabilities.py              # capability model vs. the C++ API
python3 tools/generate_test_sketch.py out/Gen    # generate a sketch from every block
```

CI ([.github/workflows/ci.yml](.github/workflows/ci.yml)) runs these and compiles every example and the generated sketch for the Nano ESP32. To add a capability, add the method to `src/Melo.h`, add an entry to `docs/CAPABILITY_MODEL.json`, and add the name to `keywords.txt`; the checker fails if they disagree.

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) and [docs/HARDWARE_CAPABILITY_INVENTORY.md](docs/HARDWARE_CAPABILITY_INVENTORY.md).

## License

MIT, see [LICENSE](LICENSE).
