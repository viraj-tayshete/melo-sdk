# Melo - Architecture

**Status:** current as of SDK 0.2.0. Replaces the Step 1 draft, which assumed an H-bridge robot and a firmware/protocol layer that were never built.

## The product

Melo is sold as a no-code kit for children and students. The customer never writes C++:

```
Melo Blocks (browser)
      │  generates Arduino code from docs/CAPABILITY_MODEL.json
      ▼
Arduino sketch  ──includes──►  Melo SDK (this library)
      │  compile (server) and upload
      ▼
Melo Mini (ESP32-S3-WROOM)  ──►  wheels (2 servos), IR, light sensor, LED, buzzer
```

**Execution model:** the generated sketch, with this library, *is* the program running on the robot. There is no separate firmware and no live link to the browser, so the earlier serial protocol (`MeloProtocol.h`) was removed.

## Layers inside the SDK

| Layer | Files | Depends on Arduino? |
|-------|-------|---------------------|
| Public API | `Melo.h`, `Melo.cpp` | yes |
| Drivers | `drive/MeloWheel`, `sensors/IRSensor`, `leds/MeloLED`, `buzzer/MeloBuzzer` | yes |
| Pure logic | `drive/WheelMath.h`, `behaviors/LineFollowLogic.h` | **no** (unit tested on the host) |
| Configuration | `MeloConfig.h`, `MeloTypes.h` | no |

Keeping the logic pure means the maths (speed to servo pulse, the line-following state machine) is tested without hardware.

## Single source of truth

`docs/CAPABILITY_MODEL.json` lists every block with its inputs, ranges, defaults and the Arduino code it generates. `tools/check_capabilities.py` fails if the JSON and `Melo.h` disagree (a public method with no capability, a wrong argument count, a mismatched error code). `tools/generate_test_sketch.py` turns every capability into code, and CI compiles it.

## Design decisions

| Decision | Reason |
|----------|--------|
| Target the ESP32-S3 (Melo Mini) on core 3.x only (Espressif's `esp32` package) | Pin-based LEDC API; the only version tested. Arduino's own `arduino:esp32` package is core 2.0.18, so the library refuses to build there with a clear message. The old 2.x code paths were untested and removed. The Blocks compile server pins its own core, so kids are unaffected. |
| Wheels are continuous-rotation servos driven by LEDC at 50 Hz, 14-bit | The kit has no motor driver. Speed is the pulse width (1500 µs = stop). |
| `stop()` removes the pulse | A continuous servo with no signal rests; configurable if a servo drifts. |
| One IR sensor, three functions | The sensor is multi-use; the user picks the meaning by the block they use. |
| Line following follows the edge | One on/off sensor cannot measure how far off the line the robot is. |
| Values are clamped, not rejected | Children should not see errors for a speed of 120. |
| Ports as a table, parts assigned to ports | Matches the six identical ports on the PCB; real pins are one edit. |
| Pins fixed at library build time | The sketch and library compile separately, so a sketch `#define` cannot reach the library. |

## Open items

- The board has six ports with an Add (ID) line each. The SDK models them as a pin table and a part-to-port list, but the routing of the ports to ESP32-S3 GPIOs, the block ID scheme, servo power and the IR/light circuits are unconfirmed (see HARDWARE_CAPABILITY_INVENTORY.md). Auto-detecting blocks is not built.
- Nothing has run on a real robot: wheel direction, stop behaviour, and the line-following timings are all untested.
- Uploading from a browser is unproven. The ESP32-S3 can be flashed over its native USB with the same protocol esptool-js speaks, so this should work in Chrome/Edge, but it depends on how the Melo Mini wires USB and its boot/reset lines.
- A runtime pin configuration (so kits with different wiring need no library edit) is not built.
