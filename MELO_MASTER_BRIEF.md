# MELO — MASTER TECHNICAL & PRODUCT BRIEF

## 0. Purpose

Melo is a robotics platform built around a custom hardware kit, an ESP-based controller, a developer SDK, and a higher-level web/block programming experience.

The central objective of the software stack is:

> Make Melo hardware programmable at the level of a robot, not at the level of microcontroller electronics.

A user should think in terms of:

`Robot → Motor → Sensor → Behavior`

rather than:

`GPIO → PWM → I²C → low-level drivers → serial packets → hardware registers`

---

# 1. System Overview

```text
                          MELO
                            │
            ┌───────────────┴───────────────┐
            │                               │
        HARDWARE                         SOFTWARE
            │                               │
       Melo PCB / ESP                 Melo Developer Stack
            │                               │
            │              ┌────────────────┼────────────────┐
            │              │                │                │
            │          ESP Firmware      Melo SDK        Melo Web
            │              │                │           / Blocks
            │              │                │                │
            └──────────────┴────── Protocol/Capability ─────┘
                                   Contract
                                        │
                                   MELO KIT
```

The practical architecture should be thought of as:

```text
User code / Melo Blocks
          ↓
     Melo API / SDK
          ↓
 Communication Protocol
          ↓
    ESP Firmware
          ↓
 Hardware drivers / peripherals
          ↓
 Motors / Servos / Sensors / LEDs / Buzzer
```

---

# 2. Ownership Model

## Viraj — Melo Arduino Library / SDK + developer-facing hardware abstraction

Primary responsibilities:

- Design and implement the Melo Arduino Library / SDK.
- Define the public developer API.
- Abstract motors, servos, sensors, LEDs, buzzer, and other hardware.
- Define initialization, calibration, status, and error behavior.
- Define or co-own the protocol contract between the SDK and ESP firmware.
- Handle the developer experience inside Arduino IDE.
- Create examples and API documentation.
- Ensure APIs can be represented consistently in Melo Blocks / web tooling.
- Coordinate with the firmware owner on supported hardware capabilities and versions.

Core principle:

> Viraj owns the software abstraction that turns Melo hardware into an easy-to-program robotics platform.

## Person 2 — Melo Web / Blocks

Primary responsibilities:

- Website UX.
- Visual blocks interface.
- Web-based programming workflow.
- Device interaction/bridge on the web side.
- Use the same conceptual Melo capabilities exposed by the SDK.

Person 2 should not independently invent a second hardware semantics layer.

## Firmware owner / hardware software owner

Primary responsibilities:

- ESP firmware.
- Low-level hardware control.
- Drivers and peripheral control.
- Firmware-level state management.
- Device protocol implementation.
- Boot/update behavior where applicable.

The firmware owner and SDK owner must agree on a stable interface contract.

---

# 3. The Most Important Architectural Principle

Separate these three layers:

## Layer A — ESP Firmware

Runs on the Melo controller.

Responsible for actually controlling the hardware.

Examples:

- set motor output
- read distance sensor
- move servo
- control LED
- read buttons/sensors
- maintain hardware safety/state

## Layer B — Melo SDK

Runs in the user's development environment and exposes a human-friendly interface.

Examples:

```cpp
#include <Melo.h>

Melo.begin();
Melo.moveForward(70);
Melo.delay(1000);
Melo.stop();
```

The SDK translates high-level API calls into the underlying device communication / hardware operations.

## Layer C — Melo Web / Blocks

Provides a visual programming experience.

Example concept:

```text
[MOVE FORWARD] [70%]
```

The Blocks experience should map to the same underlying hardware capabilities and semantics as the SDK rather than creating an unrelated command model.

---

# 4. Target Developer Experience

The desired experience is:

```text
Install Melo tools
        ↓
Connect Melo kit
        ↓
Device automatically recognized
        ↓
Correct firmware / compatibility checked
        ↓
Open example
        ↓
Upload / run
        ↓
Robot works
```

The user should not need to understand ESP GPIO mapping, PWM registers, low-level serial packets, or peripheral drivers to perform common robot tasks.

---

# 5. Melo SDK — Core Mission

The SDK is the software layer between the Melo PCB/ESP and the user.

It should hide hardware complexity while keeping enough low-level access for advanced users.

The SDK should provide two abstraction levels:

### High-level robotics APIs

Examples:

```cpp
Melo.begin();
Melo.moveForward(70);
Melo.moveBackward(50);
Melo.turnLeft(90);
Melo.turnRight(90);
Melo.stop();
```

### Component-level APIs

Examples:

```cpp
Melo.motor(LEFT, 70);
Melo.motor(RIGHT, 70);
Melo.servo(1, 90);
Melo.readDistance();
Melo.led(1, true);
Melo.beep(1000, 500);
```

Advanced users should be able to access component-level functionality without exposing unnecessary implementation details.

---

# 6. Proposed SDK Capability Areas

## 6.1 Initialization / System

Potential APIs:

```cpp
Melo.begin();
Melo.connected();
Melo.getDeviceInfo();
Melo.getFirmwareVersion();
Melo.getProtocolVersion();
Melo.status();
Melo.lastError();
```

`Melo.begin()` should conceptually perform:

1. Initialize transport.
2. Detect device.
3. Perform handshake.
4. Read device metadata.
5. Read firmware/protocol versions.
6. Validate compatibility.
7. Initialize software state.
8. Perform any required calibration/setup.
9. Return a usable status.

---

## 6.2 Motors

Potential capabilities:

- Set left motor speed.
- Set right motor speed.
- Set direction.
- Stop one motor.
- Stop all motors.
- Drive forward/backward.
- Turn left/right.
- Optional future closed-loop motion.

Possible API ideas:

```cpp
Melo.motor(LEFT, 70);
Melo.motor(RIGHT, 70);
Melo.moveForward(70);
Melo.moveBackward(70);
Melo.turnLeft(90);
Melo.turnRight(90);
Melo.stop();
```

Exact semantics must be finalized against the physical drivetrain and firmware behavior.

---

## 6.3 Servos

Potential capabilities:

```cpp
Melo.servo(1, 90);
```

Need to define:

- servo indexing
- angle range
- safe limits
- initialization behavior
- detach behavior if relevant

---

## 6.4 Sensors

Potential capabilities:

```cpp
Melo.readDistance();
Melo.readLight();
Melo.readLine();
```

Need a formal sensor registry/specification:

- sensor name/id
- data type
- units
- valid range
- sampling constraints
- error behavior
- calibration requirements

---

## 6.5 LEDs

Potential capabilities:

```cpp
Melo.led(1, true);
Melo.setLED(...);
```

Need to define whether LEDs are simple digital outputs or RGB/multi-channel devices.

---

## 6.6 Buzzer

Potential capabilities:

```cpp
Melo.beep(1000, 500);
```

Need to define:

- frequency range
- duration units
- blocking vs non-blocking behavior
- queue/overlap behavior

---

## 6.7 Calibration

Potential capabilities:

```cpp
Melo.calibrate();
Melo.calibrateMotors();
Melo.calibrateSensors();
```

Decide what calibration is:

- factory-only
- automatically performed
- required during first setup
- manually exposed to the user

Beginners should not be burdened with low-level calibration unless necessary.

---

# 7. Communication / Protocol Layer

This is one of the most critical technical interfaces.

A high-level SDK call such as:

```cpp
Melo.moveForward(70);
```

must eventually become a deterministic command understood by the ESP firmware.

The SDK ↔ firmware contract must define:

- command identifiers
- arguments
- argument types
- units
- response format
- acknowledgements
- error codes
- timeouts
- retries
- ordering guarantees where needed
- synchronous vs asynchronous operations
- device identification
- protocol versioning
- firmware compatibility

Conceptual flow:

```text
Melo.moveForward(70)
        ↓
SDK command abstraction
        ↓
Protocol encoding
        ↓
USB / Serial / chosen transport
        ↓
ESP firmware
        ↓
Motor subsystem
```

The physical protocol format can be text-based, binary, framed, or another design, but it must be explicitly specified and versioned.

---

# 8. Protocol Versioning

Do not make the SDK depend only on a firmware version number.

Track at minimum:

```text
Hardware version
Firmware version
Protocol version
SDK version
```

Example:

```text
Melo Hardware: 1.0
Firmware: 1.4.2
Protocol: 2
SDK: 0.8.0
```

The SDK should be able to determine whether the connected device is compatible and provide a meaningful error if not.

---

# 9. Device Detection

The developer experience should support:

```text
Connect device
      ↓
Device detected
      ↓
Identify Melo hardware
      ↓
Check firmware
      ↓
Connect / initialize
      ↓
Ready
```

Potential SDK concepts:

```cpp
Melo.connected();
Melo.getDeviceInfo();
Melo.getFirmwareVersion();
Melo.isCompatible();
```

The exact mechanism depends on whether the environment is direct USB serial, a desktop bridge, Web Serial, or another transport.

---

# 10. Firmware Updates / Flashing

Firmware updating and flashing are broader developer-toolchain concerns rather than just the Arduino library.

The Melo platform should eventually support:

- device detection
- firmware version detection
- compatibility checks
- firmware upload / flashing
- update status
- failure recovery
- clear user errors
- versioned firmware artifacts

These capabilities may live in Melo Studio / Melo Lab or another tooling layer rather than inside the SDK itself.

---

# 11. Serial Monitor / Diagnostics

A developer tool should eventually expose:

- serial logs
- connection status
- protocol messages where useful
- device info
- errors
- firmware information
- diagnostic/test commands

This is especially valuable during early development and field debugging.

---

# 12. Error Handling

Error behavior must be designed, not improvised.

Potential error classes:

```text
DEVICE_NOT_FOUND
CONNECTION_TIMEOUT
FIRMWARE_MISMATCH
PROTOCOL_MISMATCH
INVALID_COMMAND
INVALID_ARGUMENT
SENSOR_FAILURE
CALIBRATION_FAILED
MOTOR_ERROR
TRANSPORT_ERROR
```

The SDK should distinguish:

- recoverable errors
- user/configuration errors
- hardware errors
- connection errors
- compatibility errors

Beginner-facing messages should be human-readable.

Example:

> Melo board not detected. Check the USB connection and try again.

rather than only:

`ERR_1047`

---

# 13. Arduino IDE Integration

Melo should behave like a proper Arduino library / board ecosystem.

The basic experience should look like:

```cpp
#include <Melo.h>

void setup() {
    Melo.begin();
}

void loop() {
    Melo.moveForward(70);
    delay(1000);
    Melo.stop();
    delay(1000);
}
```

Engineering requirements include:

- library structure
- headers / implementation files
- board compatibility
- dependencies
- installation instructions
- example sketches
- semantic versioning
- README
- library metadata
- compile/upload workflow

---

# 14. Example Program Progression

The library should ship with examples that progressively teach the system.

Suggested sequence:

```text
01_BasicSetup
02_MoveForward
03_MoveBackward
04_Turn
05_Stop
06_Servo
07_DistanceSensor
08_LineSensor
09_LED
10_Buzzer
11_ObstacleAvoidance
12_SensorDrivenMovement
13_Calibration
14_Diagnostics
15_AdvancedMotorControl
```

The exact list depends on the real hardware capabilities.

---

# 15. Documentation Strategy

Documentation should have three levels.

## Level 1 — Beginner Quick Start

Answer:

> How do I get my robot running in a few minutes?

Include:

- install steps
- board setup
- library installation
- connection
- first program
- upload
- troubleshooting

## Level 2 — API Reference

For every public API document:

- purpose
- syntax
- parameters
- return value
- valid ranges
- side effects
- errors
- minimal example

## Level 3 — Developer Documentation

Document:

- architecture
- communication protocol
- firmware compatibility
- device discovery
- versioning
- debugging
- extending the SDK
- adding new hardware components

---

# 16. Shared Capability Model for Melo Blocks

The most important cross-platform principle is:

> Arduino SDK and Melo Blocks should share the same conceptual hardware capability model.

Do NOT design them as two independent robotics APIs.

For example, define a capability concept:

```text
Capability: MOVE_FORWARD
Input: speed
Range: 0–100
Effect: left motor = +speed, right motor = +speed
```

Then the same capability can be represented as:

Arduino:

```cpp
Melo.moveForward(70);
```

Blocks:

```text
[MOVE FORWARD] [70]
```

This creates a common semantic layer even if the actual implementation differs.

---

# 17. Recommended Internal SDK Architecture

A useful conceptual layering is:

```text
Public Melo API
      ↓
Robot-level abstractions
      ↓
Component abstractions
      ↓
Transport / protocol layer
      ↓
Device connection
```

Possible internal modules:

```text
Melo
├── System
├── Motors
├── Servos
├── Sensors
├── LEDs
├── Buzzer
├── Calibration
├── Errors
├── Protocol
├── Transport
└── Device
```

The user-facing API should remain stable even if low-level implementation changes.

---

# 18. Suggested Repository Structure

```text
Melo/
├── src/
│   ├── Melo.h
│   ├── Melo.cpp
│   ├── motors/
│   ├── sensors/
│   ├── servos/
│   ├── leds/
│   ├── buzzer/
│   ├── communication/
│   ├── protocol/
│   ├── calibration/
│   └── errors/
│
├── examples/
│   ├── BasicSetup/
│   ├── Movement/
│   ├── Sensors/
│   ├── Servo/
│   ├── LED/
│   ├── Buzzer/
│   └── RoboticsBehaviors/
│
├── docs/
├── library.properties
├── README.md
└── keywords.txt
```

Exact structure can change, but implementation should preserve separation between:

- public API
- device semantics
- transport
- protocol
- hardware-specific behavior

---

# 19. Development Strategy

Do not begin by implementing every possible feature.

Start with a vertical slice that proves the architecture end-to-end.

## Vertical Slice 1

Implement:

```text
Arduino user code
 ↓
Melo SDK
 ↓
Protocol
 ↓
ESP firmware
 ↓
Motor
```

Prove:

```cpp
Melo.begin();
Melo.moveForward(50);
Melo.stop();
```

## Vertical Slice 2

Add a sensor:

```cpp
Melo.readDistance();
```

Prove:

```cpp
if (Melo.readDistance() < 20) {
    Melo.stop();
}
```

## Vertical Slice 3

Add servo / LED / buzzer.

## Vertical Slice 4

Add device information, compatibility, errors, and diagnostics.

## Vertical Slice 5

Expose the same capabilities to Melo Blocks.

This sequence minimizes architectural risk.

---

# 20. Definition of Done for the SDK

The SDK should be considered meaningfully functional when a developer can:

1. Install the Melo environment.
2. Connect a Melo kit.
3. Detect the device.
4. Initialize it.
5. Run a basic example.
6. Control the motors.
7. Read at least one sensor.
8. Control at least one actuator beyond motors.
9. Understand failures through useful errors.
10. Identify device and firmware versions.
11. Use documented examples without reading the underlying firmware.
12. Use the same hardware semantics from Melo Blocks.

---

# 21. Important Decisions That Must Be Settled With the Team

Before deep implementation, explicitly resolve:

## A. What runs where?

Is the user's Arduino code running directly on the ESP, or is the ESP running separate firmware that receives commands from another host?

This fundamentally affects the SDK architecture.

## B. What is Melo Studio / Melo Lab?

Possible interpretations:

- desktop IDE
- web application
- firmware flashing/debugging tool
- Arduino integration layer
- unified developer environment

Scope should be made explicit.

## C. How does Melo Web talk to hardware?

Potential models include:

```text
Browser → Web Serial → ESP
```

or:

```text
Browser → desktop bridge → USB → ESP
```

or another architecture.

## D. What exactly does Melo Blocks consume?

Preferably it consumes a shared capability model / command specification rather than directly depending on C++ code.

## E. What hardware is actually on revision 1 of the Melo kit?

The API design must follow the actual hardware inventory.

---

# 22. Strategic Principle

Melo should not merely be “an ESP board with a library.”

The product opportunity is a complete abstraction stack:

```text
Physical robot
      ↓
Hardware abstraction
      ↓
Robot-level SDK
      ↓
Visual programming
      ↓
Beginner-friendly robotics experience
```

The strongest version of Melo allows a beginner to ignore microcontroller complexity while still giving an advanced developer enough control to build sophisticated behaviors.

---

# 23. Viraj's Immediate Priority

The immediate job is to create the SDK contract before overbuilding implementation.

The recommended order is:

```text
1. Hardware capability map
2. Public API specification
3. Firmware ↔ SDK protocol contract
4. Minimal working SDK
5. Examples
6. Documentation
7. Device/version/error handling
8. Shared capability model for Blocks
9. Developer tooling integration
```

The first milestone should be a working end-to-end movement path, not a large library with unproven architecture.

---

# 24. One-Sentence Mission

> Build the software layer that makes Melo hardware feel like a programmable robot rather than an ESP development board.
