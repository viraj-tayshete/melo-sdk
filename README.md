# Melo SDK

> **Make Melo hardware feel like a programmable robot, not an ESP development board.**

The Melo SDK is the Arduino library for the Melo robotics kit. It gives you a clean, robot-level API so you can focus on robot behavior — not GPIO, PWM, I²C, or low-level drivers.

---

## What you can do

```cpp
#include <Melo.h>

void setup() {
    Melo.begin();                  // Initialize everything
}

void loop() {
    Melo.moveForward(70);          // Drive forward at 70% speed
    delay(2000);

    Melo.stop();                   // Stop
    delay(500);

    if (Melo.obstacleDetected(20)) {
        Melo.beep(1000, 200);      // Alert beep
        Melo.turnLeft();           // Avoid obstacle
        delay(400);
    }
}
```

No `analogWrite()`. No `ledcSetup()`. No pulse timing. Just `Melo.moveForward(70)`.

---

## Installation

### Option A — Arduino Library Manager (recommended)

1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries...**
3. Search for **"Melo"**
4. Click **Install**

### Option B — Manual installation

1. Download this repository as a ZIP file
2. In Arduino IDE: **Sketch → Include Library → Add .ZIP Library...**
3. Select the downloaded ZIP file

### Option C — Developer install

```bash
git clone https://github.com/viraj-tayshete/melo-sdk.git
```
Move the `MeloSDK` folder into your Arduino `libraries/` directory.

---

## Hardware Setup

1. Connect your Melo kit via USB to your computer
2. In Arduino IDE: **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
   *(or the appropriate Melo board variant when available)*
3. **Tools → Port → [select your COM/tty port]**
4. Open **Serial Monitor** at **115200 baud**

> **Pin Configuration:** All hardware pin assignments are defined in `src/MeloConfig.h`.  
> Default values are placeholders — verify against the physical Melo PCB schematic before use.

---

## Quick Start Examples

### 1. Basic Setup
```cpp
#include <Melo.h>

void setup() {
    Melo.begin();
    Melo.printStatus();  // Print device info to Serial Monitor
}

void loop() {}
```

### 2. Drive forward and stop
```cpp
#include <Melo.h>

void setup() {
    Melo.begin();
}

void loop() {
    Melo.moveForward(70);
    delay(2000);
    Melo.stop();
    delay(1000);
}
```

### 3. Obstacle avoidance
```cpp
#include <Melo.h>

void setup() { Melo.begin(); }

void loop() {
    if (Melo.obstacleDetected(20.0)) {
        Melo.stop();
        Melo.beep(1200, 150);
        Melo.moveBackward(50);
        delay(500);
        Melo.turnRight(55);
        delay(500);
    } else {
        Melo.moveForward(60);
    }
    delay(50);
}
```

---

## API Overview

### System
| Method | Description |
|--------|-------------|
| `Melo.begin()` | Initialize robot — call first in `setup()` |
| `Melo.isReady()` | Check if initialized |
| `Melo.printStatus()` | Print device info to Serial |
| `Melo.lastErrorMessage()` | Get human-readable last error |

### Movement
| Method | Description |
|--------|-------------|
| `Melo.moveForward(speed)` | Drive forward (0–100%) |
| `Melo.moveBackward(speed)` | Drive backward (0–100%) |
| `Melo.turnLeft(speed)` | Turn left (0–100%) |
| `Melo.turnRight(speed)` | Turn right (0–100%) |
| `Melo.stop()` | Active brake all motors |
| `Melo.coast()` | Passive stop |
| `Melo.motor(id, speed)` | Set individual motor (-100 to 100%) |

### Sensors
| Method | Description |
|--------|-------------|
| `Melo.readDistance()` | Distance in cm (HC-SR04) |
| `Melo.obstacleDetected(cm)` | True if obstacle within threshold |
| `Melo.readLine()` | Line detected (true/false) |
| `Melo.readLight()` | Light level (0–4095) |

### Actuators
| Method | Description |
|--------|-------------|
| `Melo.servo(id, angle)` | Set servo angle (0–180°) |
| `Melo.servoCenter(id)` | Move servo to 90° |
| `Melo.led(id, on)` | Set LED on/off |
| `Melo.ledBlink(id, times, onMs)` | Blink LED |
| `Melo.beep(hz, ms)` | Play tone (blocking) |
| `Melo.startBeep(hz)` | Start continuous tone |
| `Melo.stopBeep()` | Stop tone |

---

## Examples

| Example | Demonstrates |
|---------|-------------|
| `01_BasicSetup` | Initialization and device info |
| `02_MoveForward` | Driving forward and stopping |
| `04_Turn` | Left and right turns |
| `06_Servo` | Servo angle control |
| `07_DistanceSensor` | Distance reading and obstacle detection |
| `09_LED` | LED on/off and blink |
| `10_Buzzer` | Tone playback |
| `11_ObstacleAvoidance` | Full autonomous behavior |

Open examples from **File → Examples → Melo** in Arduino IDE.

---

## Project Structure

```
MeloSDK/
├── src/
│   ├── Melo.h               ← Public API (include this)
│   ├── Melo.cpp             ← API implementation
│   ├── MeloConfig.h         ← Pin assignments and constants
│   ├── MeloTypes.h          ← Types, enums, structs
│   ├── motors/              ← Motor driver
│   ├── sensors/             ← Distance and line sensors
│   ├── servos/              ← Servo driver
│   ├── leds/                ← LED driver
│   ├── buzzer/              ← Buzzer driver
│   └── protocol/            ← SDK ↔ firmware protocol contract
├── examples/                ← Arduino example sketches
├── docs/
│   ├── ARCHITECTURE.md      ← System architecture
│   ├── API_REFERENCE.md     ← Full API documentation
│   ├── HARDWARE_CAPABILITY_INVENTORY.md
│   └── CAPABILITY_MODEL.json ← Shared capability model for Melo Blocks
├── library.properties
├── keywords.txt
└── README.md
```

---

## Architecture

```
User Code / Melo Blocks
        ↓
   Melo SDK / API           ← This library
        ↓
 Communication Protocol
        ↓
    ESP Firmware
        ↓
 Motors / Sensors / LEDs / Buzzer
```

See [ARCHITECTURE.md](docs/ARCHITECTURE.md) for full details.

---

## Compatibility

| Board | Status |
|-------|--------|
| ESP32 Dev Module | ✅ Primary target |
| ESP8266 | ✅ Compatible (with analogWrite / tone fallback) |
| Melo Kit v1 | ✅ Designed for |

**Dependencies:** Arduino `Servo` library (built into Arduino IDE)

---

## Important Note on Pin Configuration

> All default pin assignments in `src/MeloConfig.h` are **placeholders**.  
> They must be verified against the physical Melo PCB schematic before flashing.
>
> See `docs/HARDWARE_CAPABILITY_INVENTORY.md` for the full list of confirmed vs assumed hardware.

---

## Version

| Version | Description |
|---------|-------------|
| 0.1.0 | Initial release — full API, all subsystems, examples, docs |

See [ARCHITECTURE.md](docs/ARCHITECTURE.md) for protocol versioning details.

---

## License

MIT License — see LICENSE file.

---

## Contributing

1. Read [ARCHITECTURE.md](docs/ARCHITECTURE.md) first
2. Check [HARDWARE_CAPABILITY_INVENTORY.md](docs/HARDWARE_CAPABILITY_INVENTORY.md) for open assumptions
3. All new capabilities must be added to [CAPABILITY_MODEL.json](docs/CAPABILITY_MODEL.json)
4. Public API changes require corresponding updates to [API_REFERENCE.md](docs/API_REFERENCE.md)
