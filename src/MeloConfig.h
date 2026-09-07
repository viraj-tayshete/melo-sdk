/**
 * MeloConfig.h
 *
 * Central pin and hardware configuration for the Melo robotics platform.
 *
 * IMPORTANT: All pin assignments below are PLACEHOLDERS.
 * They must be verified and updated against the physical Melo PCB schematic
 * before flashing to real hardware.
 *
 * SDK Version: 0.1.0
 * Target: ESP32 (assumed) -- update if ESP8266 or other MCU is confirmed
 */

#ifndef MELO_CONFIG_H
#define MELO_CONFIG_H

// ---------------------------------------------------------------------------
// SDK / Protocol Versioning
// ---------------------------------------------------------------------------
#define MELO_SDK_VERSION       "0.1.0"
#define MELO_PROTOCOL_VERSION  1
#define MELO_HW_VERSION        "1.0"

// ---------------------------------------------------------------------------
// Motor Pin Configuration
// ASSUMPTION: Differential drive (left + right DC motors)
// ASSUMPTION: H-bridge driver (L298N / DRV8833 / TB6612FNG) -- unverified
//
// Each motor needs:
//   - 1 PWM pin for speed
//   - 2 direction GPIO pins (or 1 DIR + 1 PWM if using DRV8833 / TB6612FNG)
//
// Update MOTOR_DRIVER_TYPE below to match actual hardware.
// ---------------------------------------------------------------------------

// Driver types
#define MELO_DRIVER_L298N    1   // ENx + IN1/IN2 per channel
#define MELO_DRIVER_DRV8833  2   // AIN1/AIN2 with internal PWM per channel
#define MELO_DRIVER_TB6612   3   // STBY + AIN1/AIN2/PWMA per channel

#define MOTOR_DRIVER_TYPE    MELO_DRIVER_L298N  // PLACEHOLDER -- verify

// Left Motor
#define MOTOR_LEFT_PWM_PIN   14   // PLACEHOLDER
#define MOTOR_LEFT_DIR1_PIN  27   // PLACEHOLDER (IN1)
#define MOTOR_LEFT_DIR2_PIN  26   // PLACEHOLDER (IN2)

// Right Motor
#define MOTOR_RIGHT_PWM_PIN  25   // PLACEHOLDER
#define MOTOR_RIGHT_DIR1_PIN 33   // PLACEHOLDER (IN3)
#define MOTOR_RIGHT_DIR2_PIN 32   // PLACEHOLDER (IN4)

// Motor PWM channels (ESP32 LEDC)
#define MOTOR_LEFT_PWM_CH    0
#define MOTOR_RIGHT_PWM_CH   1
#define MOTOR_PWM_FREQ       5000  // Hz
#define MOTOR_PWM_RESOLUTION 8     // bits (0-255)

// ---------------------------------------------------------------------------
// Servo Configuration
// ASSUMPTION: Standard 50Hz PWM servo
// ---------------------------------------------------------------------------
#define SERVO_1_PIN          13   // PLACEHOLDER
#define SERVO_1_MIN_PULSE    500  // microseconds (0 degrees)
#define SERVO_1_MAX_PULSE    2400 // microseconds (180 degrees)
#define SERVO_COUNT          1    // PLACEHOLDER -- verify on PCB

// ---------------------------------------------------------------------------
// Distance Sensor (Ultrasonic HC-SR04 assumed)
// ---------------------------------------------------------------------------
#define ULTRASONIC_TRIGGER_PIN  5   // PLACEHOLDER
#define ULTRASONIC_ECHO_PIN     18  // PLACEHOLDER
#define ULTRASONIC_MAX_CM       400 // Maximum valid distance in cm
#define ULTRASONIC_TIMEOUT_US   25000 // Microseconds (~4m at speed of sound)

// ---------------------------------------------------------------------------
// Line Sensor
// ASSUMPTION: TCRT5000 IR reflective -- digital or analog output
// ---------------------------------------------------------------------------
#define LINE_SENSOR_PIN     19    // PLACEHOLDER
#define LINE_SENSOR_COUNT   1     // PLACEHOLDER -- may be more than 1
#define LINE_SENSOR_ANALOG  false // true = analog read; false = digital read

// ---------------------------------------------------------------------------
// Light Sensor
// ---------------------------------------------------------------------------
#define LIGHT_SENSOR_PIN    34    // PLACEHOLDER (ADC pin on ESP32)

// ---------------------------------------------------------------------------
// LED Configuration
// ASSUMPTION: Simple digital LED (not RGB)
// ---------------------------------------------------------------------------
#define LED_1_PIN           2     // PLACEHOLDER (often built-in on ESP32 devkits)
#define LED_COUNT           1     // PLACEHOLDER

// ---------------------------------------------------------------------------
// Buzzer Configuration
// ASSUMPTION: Passive buzzer, single GPIO with PWM tone()
// ---------------------------------------------------------------------------
#define BUZZER_PIN          4     // PLACEHOLDER
#define BUZZER_DEFAULT_FREQ 1000  // Hz
#define BUZZER_PWM_CH       2     // ESP32 LEDC channel for buzzer

// ---------------------------------------------------------------------------
// Button Configuration
// ---------------------------------------------------------------------------
#define BUTTON_1_PIN        0     // PLACEHOLDER (GPIO0 common on ESP32 devkits)
#define BUTTON_COUNT        1     // PLACEHOLDER

// ---------------------------------------------------------------------------
// Serial / Debug Configuration
// ---------------------------------------------------------------------------
#define MELO_SERIAL_BAUD    115200
#define MELO_DEBUG_ENABLED  true  // Set to false to suppress debug output

// ---------------------------------------------------------------------------
// Timing Constants
// ---------------------------------------------------------------------------
#define MELO_BEGIN_TIMEOUT_MS   5000  // Initialization timeout
#define MELO_SENSOR_SAMPLE_MS   50    // Minimum sensor sample interval

// ---------------------------------------------------------------------------
// Speed / Turn Constants
// These control physical behavior and should be calibrated against real hardware
// ---------------------------------------------------------------------------
#define MOTOR_MIN_SPEED_PCT    0    // 0% (stopped)
#define MOTOR_MAX_SPEED_PCT    100  // 100% (full speed)
#define TURN_DEFAULT_SPEED_PCT 60   // Speed used for turnLeft/turnRight if not specified

#endif // MELO_CONFIG_H
