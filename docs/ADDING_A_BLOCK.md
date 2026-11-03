# Adding a new sensor or block

Melo is meant to grow: a vibration sensor, more sensors, more actuators. Each addition touches the same few places, and the checks catch anything you forget. Use a made-up vibration sensor as the running example (nothing below exists in the SDK yet).

## 1. Hardware: give it a port and a pin

- Put the part on one of the six ports. Add `MELO_VIBRATION_PORT` to `src/MeloConfig.h` next to `MELO_IR_PORT`, and derive its pin the same way: `#define MELO_VIBRATION_PIN MELO_PORT_SIGNAL_PIN(MELO_VIBRATION_PORT)`.
- Add the new port to the "each part on its own port" `static_assert` in `src/Melo.cpp`.
- The pin must pass `src/PinRules.h` (no flash, USB, strapping or console pins). The build fails with a clear message if it does not.
- Record the part, its supply voltage and its output (digital/analog, polarity) in `docs/HARDWARE_CAPABILITY_INVENTORY.md`.

## 2. Driver

Create `src/sensors/VibrationSensor.h/.cpp`, a small class with `begin()` and one read function, like `IRSensor`. Keep any decision logic (a threshold, debouncing, "vibrated in the last 500 ms") in a header with **no Arduino includes**, so it is unit tested on the host like `WheelMath.h` and `LineFollowLogic.h`.

```cpp
class MeloVibrationSensor {
public:
    explicit MeloVibrationSensor(uint8_t pin);
    void begin();
    bool vibrating();   // polarity from a MELO_VIBRATION_ACTIVE_HIGH setting
};
```

## 3. Public API

In `src/Melo.h` and `src/Melo.cpp`: add the driver as a private member, initialise it in the constructor, call `begin()` in `MeloClass::begin()`, and add the public method, starting with `_checkInit("name")` like the others.

## 4. Blocks

In `docs/CAPABILITY_MODEL.json`, add a capability with `id`, `category`, `blockLabel`, `inputs`, `outputs` and a `codegen` template such as `Melo.vibrationDetected()`. If it is a diagnostic that should not become a block, add its method name to `systemApi` instead.

## 5. Everything else

- `keywords.txt`: one line per new name (real tab characters).
- `docs/API_REFERENCE.md`: a table row and the behaviour that is not obvious (polarity, units, timing).
- `examples/NN_Name/NN_Name.ino`: a short example a child could read.

## 6. Check

```sh
tests/run.sh                          # host unit tests
python3 tools/check_capabilities.py   # JSON and C++ must agree
```

Then build the examples and the generated sketch (`tools/generate_test_sketch.py out/Gen`) for **ESP32S3 Dev Module**. CI does the same on every push.

`check_capabilities.py` fails if a public method is neither a block nor in `systemApi`, if a block calls a method that does not exist or has the wrong argument count, or if an error code or a default value is wrong. That is how Blocks and the SDK are kept in step.

## Later: identifying blocks automatically

Each port has an Add line that a plug-in block can pull to ground through an ID resistor. `portOhms()` reads it. A block-type table (resistor value to part) would let `begin()` find out what is plugged in where. It is not built because the resistor values are not defined; measure them with `examples/10_PortScan` first.
