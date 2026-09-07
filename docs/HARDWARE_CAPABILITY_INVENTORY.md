# Melo — Hardware Capability Inventory

**Version:** 1.0  
**Step:** 1 — Reverse Engineering Output  
**Status:** UNVERIFIED — All entries are based on MELO_MASTER_BRIEF.md assumptions.
**Must be updated when PCB schematics / firmware code becomes available.**

---

## MOTORS

| ID | Name | Interface | Direction Control | Speed Control | Notes |
|----|------|-----------|-------------------|---------------|-------|
| M1 | Left Motor | H-bridge IC | GPIO (2 pins) | PWM (1 pin) | ASSUMED |
| M2 | Right Motor | H-bridge IC | GPIO (2 pins) | PWM (1 pin) | ASSUMED |

**Motor driver IC:** UNKNOWN — Must verify (L298N / DRV8833 / TB6612FNG)  
**Speed unit:** 0–100 (percent), mapped to 0–255 PWM internally  
**Direction values:** FORWARD / BACKWARD

---

## SERVOS

| ID | Name | Interface | Range | Center | Notes |
|----|------|-----------|-------|--------|-------|
| S1 | Servo 1 | PWM (50Hz) | 0–180 deg | 90 deg | ASSUMED |

**Servo count:** UNKNOWN — assumed 1+; must verify on PCB  
**Safe limits:** To be defined per physical servo spec

---

## SENSORS

### Distance Sensor

| Property | Value | Status |
|----------|-------|--------|
| Type | Ultrasonic (HC-SR04 assumed) | ASSUMED |
| Interface | Trigger GPIO out + Echo GPIO in | ASSUMED |
| Range | 2–400 cm | ASSUMED |
| Data type | float (cm) | ASSUMED |
| Sampling rate | Max ~20 Hz (50ms per reading) | ASSUMED |
| Error return | -1.0 (out of range / timeout) | DECIDED |

### Line Sensor

| Property | Value | Status |
|----------|-------|--------|
| Type | IR reflective (TCRT5000 assumed) | ASSUMED |
| Interface | Analog GPIO (or digital with threshold) | ASSUMED |
| Output | 0=dark/line, 1=light/no-line (or raw ADC) | ASSUMED |
| Count | UNKNOWN — 1 or more sensors | FLAG |

### Light Sensor

| Property | Value | Status |
|----------|-------|--------|
| Type | LDR / phototransistor | ASSUMED |
| Interface | Analog GPIO | ASSUMED |
| Output | Raw ADC value (0–4095 on ESP32) | ASSUMED |

### Buttons / Digital Inputs

| Property | Value | Status |
|----------|-------|--------|
| Count | UNKNOWN | FLAG: Must verify |
| Interface | GPIO with internal pull-up | ASSUMED |
| Active state | LOW when pressed | ASSUMED |

---

## LEDS

| ID | Type | Interface | Notes |
|----|------|-----------|-------|
| LED1 | Digital output (simple) | GPIO | ASSUMED — RGB unknown |

**RGB support:** UNKNOWN — simple digital assumed for now  
**Count:** UNKNOWN — at least 1

---

## BUZZER

| Property | Value | Status |
|----------|-------|--------|
| Type | Passive buzzer | ASSUMED |
| Interface | GPIO with PWM (tone()) | ASSUMED |
| Frequency range | 20–20000 Hz | ASSUMED |
| Duration unit | Milliseconds | DECIDED |
| Behavior | Blocking by default; async TBD | DECIDED |

---

## CONTROLLER / MCU

| Property | Value | Status |
|----------|-------|--------|
| Platform | ESP32 (assumed) | FLAG: Must verify exact model |
| CPU | Xtensa LX6 dual-core 240MHz (ESP32) | ASSUMED |
| Flash | 4MB (standard) | ASSUMED |
| SRAM | 520KB (ESP32) | ASSUMED |
| Arduino support | Yes (arduino-esp32 core) | ASSUMED |
| USB interface | USB-UART bridge (CP2102 / CH340) | ASSUMED |
| Serial speed | 115200 baud | DECIDED |
| PWM resolution | 8-bit (0–255) via analogWrite / ledcWrite | ASSUMED |
| ADC resolution | 12-bit (0–4095) | ASSUMED |

---

## PIN MAPPING (PLACEHOLDER)

**STATUS: ALL PINS ARE UNKNOWN — Must be verified against PCB schematic**

| Signal | GPIO | Notes |
|--------|------|-------|
| Motor Left PWM | TBD | |
| Motor Left DIR1 | TBD | |
| Motor Left DIR2 | TBD | |
| Motor Right PWM | TBD | |
| Motor Right DIR1 | TBD | |
| Motor Right DIR2 | TBD | |
| Servo 1 PWM | TBD | |
| Distance Trigger | TBD | |
| Distance Echo | TBD | |
| Line Sensor | TBD | |
| Light Sensor | TBD | |
| LED 1 | TBD | |
| Buzzer | TBD | |
| Button 1 | TBD | |

All of the above must be defined in `src/MeloConfig.h` to allow easy reconfiguration when confirmed.

---

## VERSION TRACKING

| Version Field | Value | Notes |
|--------------|-------|-------|
| Hardware Rev | 1.0 (assumed) | FLAG: confirm with team |
| Firmware Version | TBD by firmware team | |
| Protocol Version | 1 | Initial |
| SDK Version | 0.1.0 | Initial development |

---

## OPEN FLAGS (Must Resolve)

1. **Motor driver IC** — affects PWM/GPIO wiring in Motor.cpp
2. **ESP32 vs ESP8266** — affects ledcWrite vs analogWrite, ADC resolution, dual-core behavior
3. **Servo count** — affects servo API design
4. **LED type (simple vs RGB)** — affects LED API shape
5. **Line sensor count** — affects readLine() return type (single value vs array)
6. **Button count** — affects button API design
7. **Exact pin mapping** — must come from PCB schematic
8. **Execution model** — Model A vs B vs C (see ARCHITECTURE.md)
