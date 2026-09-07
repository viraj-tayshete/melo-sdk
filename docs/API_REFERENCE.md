# Melo SDK — API Reference

**SDK Version:** 0.1.0  
**Target:** ESP32 (also compatible with ESP8266)

---

## Quick Start

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

---

## System APIs

### `Melo.begin()`

Initialize the Melo robot. **Must be called first in `setup()` before any other Melo API.**

```cpp
MeloStatus status = Melo.begin();
```

| | |
|---|---|
| **Returns** | `MeloStatus` — `MELO_OK` on success, error code on failure |
| **Side effects** | Initializes all hardware subsystems, plays startup tone, blinks LED |
| **Errors** | `MELO_ERR_TRANSPORT`, `MELO_ERR_TIMEOUT` |

---

### `Melo.isReady()`

```cpp
bool ready = Melo.isReady();
```

Returns `true` if `Melo.begin()` was called successfully.

---

### `Melo.getSdkVersion()`

```cpp
String ver = Melo.getSdkVersion();  // "0.1.0"
```

---

### `Melo.getFirmwareVersion()`

```cpp
String fw = Melo.getFirmwareVersion();
```

---

### `Melo.getDeviceInfo()`

```cpp
MeloDeviceInfo info = Melo.getDeviceInfo();
Serial.println(info.hwVersion);
Serial.println(info.firmwareVersion);
Serial.println(info.sdkVersion);
Serial.println(info.protocolVersion);
Serial.println(info.isCompatible);
```

---

### `Melo.lastError()`

```cpp
MeloStatus err = Melo.lastError();
```

Returns the last error code as a `MeloStatus` enum value.

---

### `Melo.lastErrorMessage()`

```cpp
String msg = Melo.lastErrorMessage();
// Example: "Error: Sensor read failed. Check sensor connections."
```

Returns a human-readable description of the last error.

---

### `Melo.printStatus()`

```cpp
Melo.printStatus();
```

Prints device info and status to Serial Monitor. Useful for diagnostics.

---

## Robot-Level Movement APIs

### `Melo.moveForward(speed)`

Drive the robot forward.

```cpp
Melo.moveForward();      // Default 70% speed
Melo.moveForward(50);    // 50% speed
```

| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| `speed` | `uint8_t` | 0–100 | 70 | Speed as percentage of maximum |

---

### `Melo.moveBackward(speed)`

Drive the robot backward.

```cpp
Melo.moveBackward(60);
```

| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| `speed` | `uint8_t` | 0–100 | 70 | Speed as percentage of maximum |

---

### `Melo.turnLeft(speed)`

Turn the robot left (counter-clockwise) using differential drive.

```cpp
Melo.turnLeft();     // Default 60% speed
Melo.turnLeft(45);   // 45% speed
```

| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| `speed` | `uint8_t` | 0–100 | 60 | Turn speed as percentage |

---

### `Melo.turnRight(speed)`

Turn the robot right (clockwise).

```cpp
Melo.turnRight(60);
```

---

### `Melo.stop()`

Stop all motors with active braking.

```cpp
Melo.stop();
```

---

### `Melo.coast()`

Coast all motors to a stop (no active braking, motors spin down freely).

```cpp
Melo.coast();
```

---

## Component-Level Motor API

### `Melo.motor(motorId, speed)`

Set individual motor speed and direction.

```cpp
Melo.motor(MOTOR_LEFT, 80);    // Left motor forward at 80%
Melo.motor(MOTOR_RIGHT, -50);  // Right motor backward at 50%
Melo.motor(MOTOR_BOTH, 70);    // Both motors forward at 70%
```

| Parameter | Type | Values | Description |
|-----------|------|--------|-------------|
| `motorId` | `MeloMotor` | `MOTOR_LEFT`, `MOTOR_RIGHT`, `MOTOR_BOTH` | Which motor to set |
| `speed` | `int8_t` | -100 to 100 | Speed: positive=forward, negative=backward, 0=brake |

---

## Servo APIs

### `Melo.servo(servoId, angle)`

Move a servo to a target angle.

```cpp
Melo.servo(1, 90);    // Move servo 1 to 90 degrees (center)
Melo.servo(1, 0);     // Move servo 1 to 0 degrees
Melo.servo(1, 180);   // Move servo 1 to 180 degrees
```

| Parameter | Type | Range | Description |
|-----------|------|-------|-------------|
| `servoId` | `uint8_t` | 1 | Servo number (1-based) |
| `angle` | `uint8_t` | 0–180 | Target angle in degrees. Clamped to safe limits. |

---

### `Melo.servoCenter(servoId)`

Move servo to center position (90 degrees).

```cpp
Melo.servoCenter();    // Center servo 1
Melo.servoCenter(1);   // Explicit servo 1
```

---

## Sensor APIs

### `Melo.readDistance()`

Read distance from ultrasonic sensor in centimeters.

```cpp
float distance = Melo.readDistance();

if (distance < 0) {
    Serial.println("Out of range or error");
} else {
    Serial.println("Distance: " + String(distance) + " cm");
}
```

| | |
|---|---|
| **Returns** | `float` — distance in cm. `-1.0` if out of range or error |
| **Valid range** | 2.0 – 400.0 cm |
| **Blocking** | ~30ms per call |
| **Min interval** | 50ms recommended between calls |

---

### `Melo.obstacleDetected(thresholdCm)`

Check if an obstacle is closer than a threshold distance.

```cpp
if (Melo.obstacleDetected(20.0)) {
    Melo.stop();
}
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `thresholdCm` | `float` | 20.0 | Distance threshold in cm |

| | |
|---|---|
| **Returns** | `bool` — `true` if obstacle detected within threshold |

---

### `Melo.readLine()`

Read line sensor.

```cpp
bool onLine = Melo.readLine();
if (onLine) {
    // Line detected — dark surface
}
```

| | |
|---|---|
| **Returns** | `bool` — `true` if line (dark surface) detected |

---

### `Melo.readLight()`

Read ambient light level.

```cpp
uint16_t light = Melo.readLight();  // 0-4095 (ESP32 12-bit ADC)
```

| | |
|---|---|
| **Returns** | `uint16_t` — raw ADC value 0–4095. Higher = brighter. |

---

## LED APIs

### `Melo.led(ledId, on)`

Set LED state.

```cpp
Melo.led(1, true);   // LED 1 on
Melo.led(1, false);  // LED 1 off
```

| Parameter | Type | Values | Description |
|-----------|------|--------|-------------|
| `ledId` | `uint8_t` | 1 | LED number (1-based) |
| `on` | `bool` | true/false | LED state |

---

### `Melo.ledBlink(ledId, times, onMs, offMs)`

Blink LED. **Blocking.**

```cpp
Melo.ledBlink(1, 3, 200);       // LED 1, 3 blinks, 200ms on, 200ms off
Melo.ledBlink(1, 5, 100, 50);   // LED 1, 5 blinks, 100ms on, 50ms off
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ledId` | `uint8_t` | LED number (1-based) |
| `times` | `uint8_t` | Number of blinks |
| `onMs` | `uint16_t` | On duration in milliseconds |
| `offMs` | `uint16_t` | Off duration in milliseconds (default = onMs) |

---

## Buzzer APIs

### `Melo.beep(frequencyHz, durationMs)`

Play a tone. **Blocking.**

```cpp
Melo.beep(1000, 500);  // 1000 Hz for 500ms
Melo.beep(440, 200);   // A4 note for 200ms
Melo.beep(0, 500);     // Silence for 500ms
```

| Parameter | Type | Range | Description |
|-----------|------|-------|-------------|
| `frequencyHz` | `uint16_t` | 0–20000 | Tone frequency in Hz. 0 = silence. |
| `durationMs` | `uint32_t` | 0–60000 | Duration in milliseconds |

---

### `Melo.startBeep(frequencyHz)`

Start a continuous tone. **Non-blocking.** Call `Melo.stopBeep()` to end.

```cpp
Melo.startBeep(880);
delay(500);
Melo.stopBeep();
```

---

### `Melo.stopBeep()`

Stop any currently playing tone.

```cpp
Melo.stopBeep();
```

---

## Error Codes

| Code | Value | Description |
|------|-------|-------------|
| `MELO_OK` | 0 | Success |
| `MELO_ERR_NOT_INIT` | 1 | `Melo.begin()` not called |
| `MELO_ERR_DEVICE_NOT_FOUND` | 2 | Melo board not detected |
| `MELO_ERR_TIMEOUT` | 3 | Operation timed out |
| `MELO_ERR_FIRMWARE_MISMATCH` | 4 | Firmware incompatible with SDK |
| `MELO_ERR_PROTOCOL_MISMATCH` | 5 | Protocol version mismatch |
| `MELO_ERR_INVALID_ARG` | 6 | Invalid argument value |
| `MELO_ERR_INVALID_CMD` | 7 | Unknown command |
| `MELO_ERR_SENSOR` | 8 | Sensor read failure |
| `MELO_ERR_CALIBRATION` | 9 | Calibration failure |
| `MELO_ERR_MOTOR` | 10 | Motor error |
| `MELO_ERR_TRANSPORT` | 11 | Communication error |

---

## Common Patterns

### Obstacle avoidance
```cpp
void loop() {
    if (Melo.obstacleDetected(20.0)) {
        Melo.stop();
        Melo.beep(1000, 200);
        Melo.moveBackward(50);
        delay(500);
        Melo.turnRight();
        delay(400);
    } else {
        Melo.moveForward(60);
    }
    delay(50);
}
```

### Error checking
```cpp
MeloStatus status = Melo.begin();
if (status != MELO_OK) {
    Serial.println("FAILED: " + Melo.statusMessage(status));
    while (true) { delay(1000); }  // Halt
}
```

### Individual motor control
```cpp
// Gentle arc to the left
Melo.motor(MOTOR_LEFT, 30);   // Left motor slow
Melo.motor(MOTOR_RIGHT, 70);  // Right motor fast
delay(800);
Melo.stop();
```
