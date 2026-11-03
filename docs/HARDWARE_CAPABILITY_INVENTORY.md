# Melo - Hardware Inventory

Last updated 2026-11-02. **CONFIRMED** = stated by the project owner. **ASSUMED** = my working assumption. **UNKNOWN** = needs checking on the hardware.

## Controller and power

| Item | Value | Status |
|------|-------|--------|
| Board | **Melo Mini**, the team's own PCB | CONFIRMED |
| MCU module | ESP32-S3-WROOM (exact variant, e.g. N8 / N16R8, not recorded) | CONFIRMED / variant UNKNOWN |
| Arduino board setting | `ESP32S3 Dev Module` (`esp32:esp32:esp32s3`) | CHOSEN |
| Core | Espressif `esp32` package 3.x (built with 3.3.3); Arduino's `arduino:esp32` is core 2.x and unsupported | CHOSEN |
| Logic level | 3.3 V; GPIOs are not 5 V tolerant | datasheet, verify |
| Battery | 9 V | CONFIRMED |
| Power path | Power switch and step-down converters to 5 V / 3.3 V as needed; 9 V is never connected directly | CONFIRMED (design not seen) |
| USB / upload | How USB, EN and BOOT (GPIO0) are wired on the Melo Mini | UNKNOWN |

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

## Pin map (PLACEHOLDERS: the Melo Mini routing is not recorded here yet)

GPIO numbers on the ESP32-S3-WROOM. `src/PinRules.h` rejects pins the module cannot use (strapping 0/3/45/46, USB 19/20, flash/PSRAM 26-37, UART0 43/44, nonexistent 22-25).

| Signal | GPIO | Part (placeholder assignment) |
|--------|------|-------------------------------|
| Port 1 Signal / Add | 8 / 1 | IR sensor |
| Port 2 Signal / Add | 9 / 2 | Light sensor |
| Port 3 Signal / Add | 10 / 4 | LED |
| Port 4 Signal / Add | 11 / 5 | Buzzer |
| Port 5 Signal / Add | 12 / 6 | free |
| Port 6 Signal / Add | 13 / 7 | free |
| Left wheel servo | 14 | own pin (not known to use a port) |
| Right wheel servo | 15 | own pin (not known to use a port) |

Add lines are read as analog and sit on ADC1 pins (GPIO1-10).

## To find out

1. The real GPIO each pad J7-J18 reaches on the Melo Mini, and the net feeding J20 (labelled +5V on the schematic; with a 9 V battery check what it really carries).
2. Step-down design: the 5 V rail for the servos, its current rating, and whether the power switch is before it.
3. The AMS1117-3.3 on the main board: at 9 V in it dissipates about 0.57 W per 100 mA, so check its input source and heat. Also its output-capacitor requirements versus the 22 uF ceramic.
4. The ID scheme: resistor value of each block (`examples/10_PortScan` measures them).
5. Which block goes in which port; IR module type and polarity; light sensor circuit.
6. USB wiring and boot/reset circuit (needed for uploading from a browser).
7. Whether the SG90s stay still when given no signal.
