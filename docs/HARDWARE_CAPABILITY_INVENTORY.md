# Melo - Hardware Inventory

Last updated 2026-11-02. **CONFIRMED** = stated by the project owner. **ASSUMED** = my working assumption. **UNKNOWN** = needs checking on the hardware.

## Controller

| Item | Value | Status |
|------|-------|--------|
| Board | Arduino Nano ESP32 (ESP32-S3), `esp32:esp32:nano_nora` | CONFIRMED |
| Core | Espressif `esp32` package 3.x (built with 3.3.3); Arduino's `arduino:esp32` is core 2.x and unsupported | CHOSEN |
| Logic level | 3.3 V, pins not 5 V tolerant | from third-party guides, verify |
| Upload | USB DFU | from the core's board definition |

## Parts

| Part | Details | Status |
|------|---------|--------|
| Wheels | 2 x SG90, continuous rotation (360°), one per wheel | CONFIRMED |
| Drive | Differential, servos mounted mirrored | ASSUMED (flip `MELO_*_WHEEL_INVERTED` if wrong) |
| Servo creep at stop | Unknown; `stop()` cuts the pulse | UNKNOWN |
| IR sensor | 1, mounted underneath in the middle; line and obstacle use | CONFIRMED |
| IR module type | Digital or analog, polarity | UNKNOWN |
| Light sensor | Present | CONFIRMED |
| Light circuit | Output direction, ADC-capable pin | UNKNOWN |
| LED | 1 simple white LED, active high | CONFIRMED / ASSUMED polarity |
| Buzzer | Present; assumed passive | CONFIRMED / ASSUMED type |
| Motor driver, DC motors | None | CONFIRMED |
| Buttons, distance sensor, IMU, battery sensing | None | CONFIRMED |

## Board (PCB), from the KiCad schematics

Source: `MELOlogic.kicad_sch` (main board) and `MELOlogicblock.kicad_sch` ("Melo Block PCB", 25-08-2026). Connections were computed by script; no electrical rules check was run.

**Main board**

| Item | Detail |
|------|--------|
| Ports | Six identical 4-pin ports J1-J6 (2.00 mm headers) |
| Port pin 1 | Signal, to its own pad J13-J18 |
| Port pin 2 | "Add", to its own pad J7-J12, with a 10k pull-up to 3.3 V (R7..R2) |
| Port pin 3 / 4 | 3.3 V / GND |
| Regulator | U2 AMS1117-3.3 (SOT-223); C1 10 uF on the +5 V input, C2 22 uF on the 3.3 V output |
| Power input | J20 "Vin pin" (+5 V), J19 GND pin |
| Power LED | D1 with R1 1k5 on 3.3 V (about 1 mA) |
| Controller | **No Nano symbol.** The 14 pads J7-J20 (6 Add, 6 Signal, GND, +5 V) connect to it; the mapping is unknown |

Port to pad: J1 = Signal J13 / Add J7, J2 = J14 / J8, J3 = J15 / J9, J4 = J16 / J10, J5 = J17 / J11, J6 = J18 / J12.

**Block module:** passes Signal, 3.3 V and GND from a JST PH socket to a header, and connects one resistor R1 (value not set) from the Add pin to GND. With the 10k pull-up this forms a voltage divider. That it is meant to identify the block is an inference; the scheme is not defined.

**Open electrical questions**

- The ports only carry 3.3 V. The SG90 servos are specified for about 4.8-6 V, and the schematic shows no 5 V reaching any port, so how the wheel servos are powered is unknown.
- What supplies J20's +5 V (the Nano ESP32 has no ordinary 5 V output pin).
- AMS1117 output-capacitor requirements versus the 22 uF ceramic: check the datasheet.

## Pin map (PLACEHOLDERS: the pad-to-controller mapping is unknown)

| Signal | Nano ESP32 pin | Part (placeholder assignment) |
|--------|----------------|-------------------------------|
| Port 1 Signal / Add | D2 / A0 | IR sensor |
| Port 2 Signal / Add | D3 / A1 | Light sensor |
| Port 3 Signal / Add | D4 / A2 | LED |
| Port 4 Signal / Add | D5 / A3 | Buzzer |
| Port 5 Signal / Add | D6 / A4 | free |
| Port 6 Signal / Add | D7 / A5 | free |
| Left wheel servo | D8 | own pin (not known to use a port) |
| Right wheel servo | D9 | own pin (not known to use a port) |

## To find out

1. Which controller pin each pad J7-J18 reaches, and the source of J20's +5 V.
2. How the wheel servos are powered and connected (ports or separate connectors).
3. The ID scheme: resistor value of each block. `examples/10_PortScan` measures them on the real board once the pins are right.
4. Which block goes in which port.
5. IR module type and output polarity, and the light sensor circuit.
6. Whether the SG90s stay still when given no signal.
