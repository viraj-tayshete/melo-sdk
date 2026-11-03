/**
 * MeloConfig.h
 *
 * Central pin and hardware configuration for the Melo Mini robot board.
 *
 * Target: ESP32-S3-WROOM on the Melo Mini PCB (Tools > Board > esp32 >
 * ESP32S3 Dev Module), ESP32 Arduino core 3.x. Pins are GPIO numbers.
 *
 * IMPORTANT: every pin below is a PLACEHOLDER until it is checked against the
 * real Melo Mini PCB. See PinRules.h for the pins the module cannot use.
 *
 * To change a pin, edit the MELO_DEFAULT_* lines below. (A #define in a sketch
 * does NOT work: the library is compiled separately from the sketch. Passing
 * -DMELO_PORT3_SIGNAL_PIN=40 as a compiler flag for the whole build does work.)
 *
 * SDK Version: 0.2.0
 */

#ifndef MELO_CONFIG_H
#define MELO_CONFIG_H

#define MELO_SDK_VERSION  "0.2.0"
#define MELO_HW_VERSION   "1.0"   // ASSUMED, not confirmed with the hardware team

// ---------------------------------------------------------------------------
// Ports (from MELOlogic.kicad_sch)
//
// The board has six identical 4-pin ports, J1-J6. Each carries:
//   pin 1  Signal  -> one controller pin (pads J13-J18)
//   pin 2  Add     -> one controller pin (pads J7-J12), 10k pull-up to 3.3V
//   pin 3  3.3V      pin 4  GND
// A plug-in block may put a resistor from Add to GND, so the voltage on the
// Add pin tells which block is plugged in (this is an inference; the ID scheme
// is not defined yet). Portn below means port Jn.
//
// !! PLACEHOLDERS !! The real Melo Mini routing is not recorded here yet.
// ---------------------------------------------------------------------------
#if defined(ARDUINO) && !defined(CONFIG_IDF_TARGET_ESP32S3)
  #error "Melo targets the ESP32-S3 (Melo Mini). Choose Tools > Board > esp32 > ESP32S3 Dev Module."
#endif

// GPIO numbers on the ESP32-S3-WROOM. The Add lines are read as analog, so
// they sit on ADC1 pins (GPIO1-10), which also work together with Wi-Fi.
#define MELO_DEFAULT_PORT1_SIGNAL_PIN  8
#define MELO_DEFAULT_PORT2_SIGNAL_PIN  9
#define MELO_DEFAULT_PORT3_SIGNAL_PIN  10
#define MELO_DEFAULT_PORT4_SIGNAL_PIN  11
#define MELO_DEFAULT_PORT5_SIGNAL_PIN  12
#define MELO_DEFAULT_PORT6_SIGNAL_PIN  13
#define MELO_DEFAULT_PORT1_ADD_PIN     1
#define MELO_DEFAULT_PORT2_ADD_PIN     2
#define MELO_DEFAULT_PORT3_ADD_PIN     4
#define MELO_DEFAULT_PORT4_ADD_PIN     5
#define MELO_DEFAULT_PORT5_ADD_PIN     6
#define MELO_DEFAULT_PORT6_ADD_PIN     7
#define MELO_DEFAULT_LEFT_WHEEL_PIN    14
#define MELO_DEFAULT_RIGHT_WHEEL_PIN   15

#ifndef MELO_PORT1_SIGNAL_PIN
  #define MELO_PORT1_SIGNAL_PIN  MELO_DEFAULT_PORT1_SIGNAL_PIN
#endif
#ifndef MELO_PORT2_SIGNAL_PIN
  #define MELO_PORT2_SIGNAL_PIN  MELO_DEFAULT_PORT2_SIGNAL_PIN
#endif
#ifndef MELO_PORT3_SIGNAL_PIN
  #define MELO_PORT3_SIGNAL_PIN  MELO_DEFAULT_PORT3_SIGNAL_PIN
#endif
#ifndef MELO_PORT4_SIGNAL_PIN
  #define MELO_PORT4_SIGNAL_PIN  MELO_DEFAULT_PORT4_SIGNAL_PIN
#endif
#ifndef MELO_PORT5_SIGNAL_PIN
  #define MELO_PORT5_SIGNAL_PIN  MELO_DEFAULT_PORT5_SIGNAL_PIN
#endif
#ifndef MELO_PORT6_SIGNAL_PIN
  #define MELO_PORT6_SIGNAL_PIN  MELO_DEFAULT_PORT6_SIGNAL_PIN
#endif
#ifndef MELO_PORT1_ADD_PIN
  #define MELO_PORT1_ADD_PIN     MELO_DEFAULT_PORT1_ADD_PIN
#endif
#ifndef MELO_PORT2_ADD_PIN
  #define MELO_PORT2_ADD_PIN     MELO_DEFAULT_PORT2_ADD_PIN
#endif
#ifndef MELO_PORT3_ADD_PIN
  #define MELO_PORT3_ADD_PIN     MELO_DEFAULT_PORT3_ADD_PIN
#endif
#ifndef MELO_PORT4_ADD_PIN
  #define MELO_PORT4_ADD_PIN     MELO_DEFAULT_PORT4_ADD_PIN
#endif
#ifndef MELO_PORT5_ADD_PIN
  #define MELO_PORT5_ADD_PIN     MELO_DEFAULT_PORT5_ADD_PIN
#endif
#ifndef MELO_PORT6_ADD_PIN
  #define MELO_PORT6_ADD_PIN     MELO_DEFAULT_PORT6_ADD_PIN
#endif

// Wheel servos: not known whether they plug into ports, so they have own pins.
#ifndef MELO_LEFT_WHEEL_PIN
  #define MELO_LEFT_WHEEL_PIN   MELO_DEFAULT_LEFT_WHEEL_PIN
#endif
#ifndef MELO_RIGHT_WHEEL_PIN
  #define MELO_RIGHT_WHEEL_PIN  MELO_DEFAULT_RIGHT_WHEEL_PIN
#endif

#define MELO_PORT_COUNT         6
#define MELO_PORT_PULLUP_OHMS   10000   // R2-R7 on the schematic

// Which port each part is plugged into (PLACEHOLDERS; ports 5 and 6 are free).
#ifndef MELO_IR_PORT
  #define MELO_IR_PORT      1
#endif
#ifndef MELO_LIGHT_PORT
  #define MELO_LIGHT_PORT   2
#endif
#ifndef MELO_LED_PORT
  #define MELO_LED_PORT     3
#endif
#ifndef MELO_BUZZER_PORT
  #define MELO_BUZZER_PORT  4
#endif

// Token pasting that expands its arguments first (MELO_IR_PORT -> 1 -> MELO_PORT1_...).
#define MELO_CAT3_(a, b, c)  a##b##c
#define MELO_CAT3(a, b, c)   MELO_CAT3_(a, b, c)
#define MELO_PORT_SIGNAL_PIN(n)  MELO_CAT3(MELO_PORT, n, _SIGNAL_PIN)
#define MELO_PORT_ADD_PIN(n)     MELO_CAT3(MELO_PORT, n, _ADD_PIN)

#define MELO_IR_PIN      MELO_PORT_SIGNAL_PIN(MELO_IR_PORT)
#define MELO_LIGHT_PIN   MELO_PORT_SIGNAL_PIN(MELO_LIGHT_PORT)   // must be ADC-capable
#define MELO_LED_PIN     MELO_PORT_SIGNAL_PIN(MELO_LED_PORT)
#define MELO_BUZZER_PIN  MELO_PORT_SIGNAL_PIN(MELO_BUZZER_PORT)

// ---------------------------------------------------------------------------
// Wheels: two continuous-rotation (360 degree) SG90-type servos, one per wheel.
// A continuous servo reads the pulse width as SPEED: ~1500us = stopped,
// shorter = one direction, longer = the other.
// ---------------------------------------------------------------------------
#define MELO_SERVO_FREQ_HZ     50
#define MELO_SERVO_PERIOD_US   20000
#define MELO_SERVO_RES_BITS    14
#define MELO_SERVO_CENTER_US   1500   // stopped
#define MELO_SERVO_SPAN_US     500    // full speed = center +/- span (1000..2000us)
#define MELO_SERVO_MIN_US      1000   // hard safety clamp
#define MELO_SERVO_MAX_US      2000

// The wheels face opposite ways, so one servo must turn the other way round for
// the robot to go forward. ASSUMED: left normal, right mirrored. If the robot
// spins on the spot when you call moveForward(), flip both of these.
#ifndef MELO_LEFT_WHEEL_INVERTED
  #define MELO_LEFT_WHEEL_INVERTED   false
#endif
#ifndef MELO_RIGHT_WHEEL_INVERTED
  #define MELO_RIGHT_WHEEL_INVERTED  true
#endif

// Per-servo centre correction in microseconds, for servos that creep when "stopped".
#ifndef MELO_LEFT_WHEEL_TRIM_US
  #define MELO_LEFT_WHEEL_TRIM_US    0
#endif
#ifndef MELO_RIGHT_WHEEL_TRIM_US
  #define MELO_RIGHT_WHEEL_TRIM_US   0
#endif

// 1 = stop() removes the pulse completely (a servo with no signal normally rests).
// 0 = stop() sends the centre pulse (centre + trim). Use 0 if a servo drifts with no signal.
#ifndef MELO_STOP_CUTS_PULSE
  #define MELO_STOP_CUTS_PULSE   1
#endif

#define MELO_TURN_DEFAULT_SPEED_PCT  60
#define MELO_MOVE_DEFAULT_SPEED_PCT  70

// ---------------------------------------------------------------------------
// IR sensor (type and wiring UNCONFIRMED). One sensor serves three roles:
//   irDetected()       a surface reflects IR back
//   lineDetected()     over a dark line (no reflection)
//   obstacleDetected() something reflects IR in front
// ---------------------------------------------------------------------------
#ifndef MELO_IR_ANALOG
  #define MELO_IR_ANALOG            0      // 0 = digital module output, 1 = analog output
#endif
// Common TCRT5000 / LM393 modules pull the output LOW when they see a reflection.
#ifndef MELO_IR_REFLECT_IS_LOW
  #define MELO_IR_REFLECT_IS_LOW    1
#endif
#ifndef MELO_IR_ANALOG_THRESHOLD
  #define MELO_IR_ANALOG_THRESHOLD  2000   // 12-bit ADC; only used when MELO_IR_ANALOG is 1
#endif

// ---------------------------------------------------------------------------
// Light sensor (circuit UNCONFIRMED)
// ---------------------------------------------------------------------------
#ifndef MELO_LIGHT_BRIGHT_IS_HIGH
  #define MELO_LIGHT_BRIGHT_IS_HIGH  1     // 0 if the reading falls as light rises
#endif
#define MELO_ADC_MAX  4095

// ---------------------------------------------------------------------------
// LED and buzzer
// ---------------------------------------------------------------------------
#ifndef MELO_LED_ACTIVE_HIGH
  #define MELO_LED_ACTIVE_HIGH  true
#endif
#define MELO_BUZZER_MIN_HZ  20
#define MELO_BUZZER_MAX_HZ  20000

// ---------------------------------------------------------------------------
// Line following (single IR sensor). UNTUNED: tune on the real track.
// ---------------------------------------------------------------------------
#define MELO_LINE_DEFAULT_SPEED_PCT  50
#define MELO_LINE_INNER_PCT          15    // slow wheel speed while steering, as % of speed
#define MELO_LINE_LOST_MS            1200  // no line for this long = lost
#define MELO_LINE_SEARCH_STEP_MS     350   // first search sweep; each sweep is this much longer
#define MELO_LINE_SEARCH_MAX_MS      6000  // give up after searching this long
#define MELO_LINE_SEARCH_SPEED_PCT   40
#define MELO_LINE_STEP_MS            5     // pause per followLine() call
#define MELO_LINE_NEW_RUN_GAP_MS     500   // a pause longer than this starts a fresh run

// ---------------------------------------------------------------------------
// Serial / debug
// ---------------------------------------------------------------------------
#define MELO_SERIAL_BAUD    115200
#ifndef MELO_DEBUG_ENABLED
  #define MELO_DEBUG_ENABLED  true
#endif

#endif // MELO_CONFIG_H
