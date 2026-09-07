# Melo — Architecture Contract

**Document Version:** 1.0  
**Status:** Draft — Step 1 Output  
**Author:** Viraj (SDK Owner)  
**Date:** 2026-09-07

---

## 1. Purpose of This Document

This document is the result of Step 1 — Reverse Engineering the Melo system and establishing the Architecture Contract.

It describes:
- The current state of the repository (what exists)
- The target architecture (what will be built)
- Hardware capabilities identified
- Assumptions flagged explicitly
- Risks and dependencies
- Recommended implementation order

---

## 2. Repository State at Step 1

### What Exists

| Item | Status | Notes |
|------|--------|-------|
| Planning documents | EXISTS | MELO_MASTER_BRIEF.md, MELO_CODEX_CLAUDE_CODE_PROMPTS.md |
| Arduino library source | NOT YET | To be created in Step 2 |
| ESP firmware | NOT PRESENT | Expected from firmware team |
| Protocol definition | NOT FORMAL | To be designed in Step 3 |
| Examples | NOT YET | To be created in Step 2 |
| Documentation | NOT YET | This document is Step 1 output |
| Build system | NOT CONFIGURED | library.properties to be created |
| Tests | NONE | To be created in Step 6 |

### Greenfield Status

This is a greenfield Arduino library project. No prior SDK, protocol, or firmware code exists in this repository. All SDK work begins from scratch, guided by the architecture described in MELO_MASTER_BRIEF.md.

---

## 3. Hardware Capability Inventory

> ASSUMPTION FLAG: The following hardware inventory is derived from MELO_MASTER_BRIEF.md.
> It has NOT been verified against physical hardware schematics or firmware code.
> When hardware schematics / firmware become available, this table MUST be updated.

### 3.1 Motion / Drivetrain

| Component | Type | Assumed Details | Verified |
|-----------|------|-----------------|---------|
| Left Motor | DC Motor | Speed via PWM, direction via H-bridge GPIO | ASSUMED |
| Right Motor | DC Motor | Speed via PWM, direction via H-bridge GPIO | ASSUMED |
| Motor Driver | H-Bridge IC | Unknown IC (L298N / DRV8833 / TB6612FNG) | FLAG: Must confirm |
| Drivetrain | Differential / Tank | Left + Right independent | ASSUMED |

### 3.2 Servos

| Component | Type | Assumed Details | Verified |
|-----------|------|-----------------|---------|
| Servo 1 | Standard PWM servo | Range 0-180 deg, 50Hz PWM | ASSUMED |
| Servo count | Unknown | Brief mentions at least 1 | FLAG |

### 3.3 Sensors

| Component | Type | Assumed Details | Verified |
|-----------|------|-----------------|---------|
| Distance sensor | Ultrasonic HC-SR04 assumed | Trigger + Echo GPIO, range 2-400 cm | ASSUMED |
| Line sensor | IR reflective TCRT5000 assumed | Analog or digital GPIO input | ASSUMED |
| Light sensor | LDR / Phototransistor | Analog GPIO | ASSUMED |
| Button(s) | Digital input | GPIO with internal pull-up | FLAG: count unknown |

### 3.4 Output Peripherals

| Component | Type | Assumed Details | Verified |
|-----------|------|-----------------|---------|
| LED(s) | Simple digital or RGB | GPIO HIGH/LOW; RGB capability unknown | FLAG: RGB vs simple? |
| Buzzer | Passive buzzer | PWM tone generation via tone() | ASSUMED |

### 3.5 Controller

| Component | Details | Verified |
|-----------|---------|---------|
| MCU | ESP32 or ESP8266 (exact model unknown) | FLAG: Must confirm |
| USB interface | USB-UART bridge (CP2102 / CH340 assumed) | ASSUMED |
| Flash method | Arduino IDE + esptool via USB | ASSUMED |
| Logic voltage | 3.3V logic (ESP) | ASSUMED |

---

## 4. Target Architecture

```
+-----------------------------------------------------------+
|                     USER LAYER                            |
|    Arduino Sketch  /  Melo Blocks  /  Web Interface       |
+---------------------------+-------------------------------+
                            | uses
+---------------------------v-------------------------------+
|                   MELO SDK (Layer B)                      |
|   Melo.h / Melo.cpp  Public API                          |
|                                                           |
|   [Motors]  [Sensors]  [Servos]  [LEDs]  [Buzzer]        |
|       |          |         |        |        |            |
|       +----------+---------+--------+--------+            |
|                            |                              |
|              [Protocol Layer]                             |
|       Command encoding, versioning, error codes           |
|                            |                              |
|              [Transport Layer]                            |
|         Serial / USB / WebSerial abstraction              |
+---------------------------+-------------------------------+
                            | serial/USB
+---------------------------v-------------------------------+
|                ESP FIRMWARE (Layer A)                     |
|   Command parser, hardware drivers, peripheral control    |
+---------------------------+-------------------------------+
                            | controls
+---------------------------v-------------------------------+
|               MELO HARDWARE (PCB)                         |
|   Motors, Servos, Sensors, LEDs, Buzzer                  |
+-----------------------------------------------------------+
```

---

## 5. Execution Model Decision

> FLAG -- Must be confirmed against actual hardware and firmware architecture.

### Model A (Chosen) -- Direct Arduino Library on ESP

```
User writes Arduino sketch
    -> includes <Melo.h>
    -> compiles and uploads sketch to ESP
    -> Melo.h wraps Arduino GPIO / PWM / I2C APIs directly
```

- SDK IS the firmware. User code runs ON the ESP.
- Simplest model. Consistent with the Arduino IDE examples in MELO_MASTER_BRIEF.md.
- Drawback: Updating SDK requires re-uploading sketch.

### Model B -- Host SDK + Permanent Firmware

```
User writes host code
    -> SDK sends structured commands over serial
    -> ESP runs permanent firmware (command interpreter)
```

- More flexible, enables Melo Blocks / Web natively.
- More complex. Requires stable protocol contract.

### Model C -- Hybrid

```
Arduino sketch includes <Melo.h>
    -> SDK sends commands to ESP co-processor
```

DECISION: Proceed with Model A. Design transport and protocol layers as internal abstractions to allow future migration to Model B without breaking the public API surface.

---

## 6. Dependency and Risk Register

| ID | Risk | Severity | Mitigation |
|----|------|----------|-----------|
| R1 | Execution model unknown | HIGH | Proceed Model A; keep transport-agnostic |
| R2 | Motor driver IC unknown | MEDIUM | Abstract via MotorDriver interface |
| R3 | ESP model unknown | MEDIUM | Use common Arduino APIs only |
| R4 | No firmware code in repo | MEDIUM | SDK in Model A IS the firmware |
| R5 | Sensor pin assignments unknown | MEDIUM | Use MeloConfig.h for all pin choices |
| R6 | LED type (simple vs RGB) unknown | LOW | Write simple digital first; add RGB later |
| R7 | Protocol versioning without firmware | LOW | Define version scheme; leave validation placeholder |

---

## 7. Vertical Slices (Smallest Provable Paths)

### Slice 1 -- Drivetrain (FIRST)
```cpp
Melo.begin();
Melo.moveForward(70);
delay(1000);
Melo.stop();
```
Proves: library installs, GPIO/PWM init, motor control, stop.

### Slice 2 -- Distance Sensor
```cpp
float d = Melo.readDistance();
if (d < 20) Melo.stop();
```
Proves: sensor read path works.

### Slice 3 -- Servo + LED + Buzzer
```cpp
Melo.servo(1, 90);
Melo.led(1, true);
Melo.beep(1000, 500);
```

### Slice 4 -- Device Info + Errors
```cpp
Melo.begin();
String ver = Melo.getFirmwareVersion();
String err = Melo.lastError();
```

### Slice 5 -- Shared Capability Model (JSON for Melo Blocks)

---

## 8. Assumptions Summary

| ID | Assumption | Impact if Wrong |
|----|-----------|----------------|
| A1 | Model A: user code runs on ESP | Major SDK rewrite |
| A2 | Differential tank drive, 2 DC motors | API shape changes |
| A3 | HC-SR04 for distance sensor | Pin config / driver changes |
| A4 | TCRT5000 for line sensor | Driver changes |
| A5 | Passive buzzer, single GPIO | tone() API changes |
| A6 | ESP32 target platform | Board config changes |
| A7 | USB-UART bridge for serial/flash | Toolchain setup changes |
| A8 | Simple digital LEDs (not RGB) | LED API shape changes |

---

## 9. Recommended Implementation Order

```
Step 2 -- Implement SDK (Layer B):
  1. Library scaffold: Melo.h, Melo.cpp, library.properties
  2. MeloConfig.h -- pin assignments, hardware selectors
  3. Motor subsystem (Motor.h / Motor.cpp)
  4. High-level API: moveForward, moveBackward, turnLeft, turnRight, stop
  5. DistanceSensor subsystem
  6. LineSensor subsystem
  7. Servo subsystem
  8. LED subsystem
  9. Buzzer subsystem
  10. Examples for each capability
  11. README + keywords.txt

Step 3 -- Formalize Protocol (Layer between B and A):
  1. Command identifiers (enum)
  2. Argument types, ranges, units
  3. Error codes and human-readable messages
  4. Version constants (SDK_VERSION, PROTOCOL_VERSION)
  5. Compatibility check in Melo.begin()

Step 4 -- Developer Tooling:
  1. Diagnostic example sketch
  2. Serial debug output
  3. Device info display

Step 5 -- Shared Capability Model:
  1. JSON schema for all capabilities
  2. Validation tooling

Step 6 -- Tests + Hardening:
  1. Unit tests (host-side where possible)
  2. Integration tests
  3. Final API reference
```

---

## 10. Next Step

Proceed to Step 2 -- Design and Implement the Melo SDK + Public API.
Start with Vertical Slice 1: Melo.begin() -> moveForward(speed) -> stop().
