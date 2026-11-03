# Melo SDK - API Reference

**SDK version:** 0.2.0  **Board:** Melo Mini, ESP32-S3-WROOM (ESP32 core 3.x)

Speeds are percentages of full wheel speed. Calling any function before `Melo.begin()` does nothing, prints a message to Serial and sets `lastError()` to `MELO_ERR_NOT_INIT`.

## System

| Function | Description |
|----------|-------------|
| `MeloStatus begin()` | Set up the robot; call first in `setup()`. Plays a startup sound and blinks the LED. Returns `MELO_OK`, or `MELO_ERR_PIN` if a wheel pin could not be set up (and plays an error sound). Safe to call twice. |
| `bool isReady()` | `true` after a successful `begin()`. |
| `String getSdkVersion()` | e.g. `"0.2.0"`. |
| `MeloDeviceInfo getDeviceInfo()` | `sdkVersion`, `hwVersion`, `board`. |
| `MeloStatus lastError()` | Last error code. |
| `String lastErrorMessage()` | Last error in plain language. |
| `String statusMessage(MeloStatus)` | Plain-language text for any code. |
| `void printStatus()` | Print version, board and last error to Serial. |

## Driving

| Function | Range | Description |
|----------|-------|-------------|
| `moveForward(speed = 70)` | 0-100 | Both wheels forward until stopped. |
| `moveBackward(speed = 70)` | 0-100 | Both wheels backward. |
| `turnLeft(speed = 60)` | 0-100 | Spin left on the spot. **A speed, not an angle**: how far it turns depends on how long you wait. |
| `turnRight(speed = 60)` | 0-100 | Spin right on the spot. |
| `stop()` | | Stop both wheels. |
| `wheel(side, speed)` | -100..100 | One wheel (`WHEEL_LEFT`, `WHEEL_RIGHT`) or `WHEEL_BOTH`. Negative = backward, 0 = stop. |
| `trimWheel(side, trimUs)` | -200..200 | Add microseconds to the centre pulse, for a servo that creeps when stopped. Takes effect on the next drive command. |

Values above the range are clamped. `stop()` removes the servo pulse entirely (`MELO_STOP_CUTS_PULSE`); if a servo drifts with no signal, set it to 0 to send the centre pulse instead.

## Line following

```cpp
while (Melo.followLine(50)) { }
```

| Function | Description |
|----------|-------------|
| `bool followLine(speed = 50)` | One short control step (about 5 ms); call it in a loop. Returns `false` when the line was lost and could not be found again: the wheels stop, an error sound plays and `lastError()` is `MELO_ERR_LINE_LOST`. A pause of over 0.5 s between calls starts a fresh run. |
| `void resetLineFollow()` | Start from scratch. |

Assumes a dark line on a light floor, with the sensor facing down **in front of** the wheels. With one on/off sensor the robot follows the *edge* of the line (arcs one way on the line, the other way off it); a second sensor would be smoother. If it sees no line for 1.2 s it sweeps left and right with growing sweeps, and gives up after 6 s. All timings are untuned (see `MeloConfig.h`).

## IR sensor

One sensor, three meanings.

| Function | Returns |
|----------|---------|
| `irDetected()` | `true` if the sensor sees a reflection. |
| `lineDetected()` | `true` if over a dark line (no reflection). The opposite of `irDetected()`. |
| `obstacleDetected()` | `true` if something reflects IR in front. **Not a distance**: range is set by the module's trimmer. |
| `irRaw()` | 0/1 (digital module) or 0-4095 (analog module, `MELO_IR_ANALOG`). |

Many modules pull their output LOW on reflection (`MELO_IR_REFLECT_IS_LOW`, the default). Test yours with `04_IRSensor`; if the answers are backwards, flip it. An unplugged sensor cannot be detected: a floating pin gives unpredictable readings.

## Ports (diagnostics)

The board has six identical ports. A plug-in block may put a resistor from the port's Add line to ground; the board pulls the line up to 3.3 V with 10k, so the voltage shows which block is plugged in. The ID scheme is not defined yet; these two functions exist to measure it.

| Function | Returns |
|----------|---------|
| `uint16_t portRaw(port)` | Raw reading 0-4095 of the Add line, port 1-6. Near 4095 with nothing plugged in. |
| `int32_t portOhms(port)` | Estimated resistor in ohms; `0` = shorted, `-1` = nothing plugged in. An estimate (the ESP32 ADC is not linear). |

A port outside 1-6 sets `MELO_ERR_INVALID_ARG`. See `examples/10_PortScan`.

## Light sensor

| Function | Returns |
|----------|---------|
| `uint8_t readLight()` | 0 (dark) to 100 (bright). |
| `uint16_t readLightRaw()` | 0-4095. |

If the reading falls as light rises, set `MELO_LIGHT_BRIGHT_IS_HIGH` to 0.

## LED

| Function | Description |
|----------|-------------|
| `led(bool on)` | On or off. |
| `ledBlink(times, onMs, offMs = 0)` | Blink (blocking). `offMs` 0 means the same as `onMs`. |

## Buzzer

| Function | Description |
|----------|-------------|
| `beep(frequencyHz, durationMs)` | Play a tone and wait (blocking). 20-20000 Hz; 0 = silence. |
| `startBeep(frequencyHz)` | Start a continuous tone (non-blocking). |
| `stopBeep()` | Stop it. |

## Program

| Function | Description |
|----------|-------------|
| `halt()` | Stop the wheels, silence the buzzer, turn the LED off, then wait forever. Melo Blocks ends every program with it so the robot never keeps driving. |

## Status codes

| Code | Value | Meaning |
|------|-------|---------|
| `MELO_OK` | 0 | Success |
| `MELO_ERR_NOT_INIT` | 1 | `begin()` not called, or it failed |
| `MELO_ERR_INVALID_ARG` | 2 | A port number outside 1-6 (other out-of-range values are clamped instead) |
| `MELO_ERR_PIN` | 3 | A pin could not be set up |
| `MELO_ERR_LINE_LOST` | 4 | `followLine()` gave up |

## Configuration

All in [src/MeloConfig.h](../src/MeloConfig.h): the port pin table (`MELO_DEFAULT_PORTn_*`), which port each part is in (`MELO_IR_PORT`, ...), wheel pins, wheel direction (`MELO_*_WHEEL_INVERTED`), servo trim, IR and light polarity, line-following timings. A `#define` in a sketch does **not** change them, because the library is compiled separately; edit the header, or pass `-DNAME=value` as a compiler flag for the whole build.
