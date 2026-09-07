# MELO — CODEX / CLAUDE CODE EXECUTION PROMPTS

This document contains high-level prompts intended to be pasted into Codex or Claude Code. Each phase is deliberately written as an execution brief rather than as a detailed implementation recipe. The coding agent should inspect the existing repository, infer details from the actual hardware/firmware code, and implement the phase while documenting assumptions.

---

# GLOBAL MASTER PROMPT

You are the lead software engineer for the Melo robotics platform.

Your job is to build a production-quality developer stack for an ESP-based robotics kit called Melo.

The product goal is:

> Make Melo hardware programmable at the level of a robot, not at the level of microcontroller electronics.

Users should be able to write simple code such as:

```cpp
#include <Melo.h>

Melo.begin();
Melo.moveForward(70);
Melo.stop();
```

without needing to understand GPIO, PWM, I²C, low-level registers, or internal hardware wiring.

Architect the system around these layers:

```text
User / Melo Blocks
        ↓
Melo SDK / Public API
        ↓
Transport + Protocol
        ↓
ESP Firmware
        ↓
Hardware
```

Before changing code:

1. Inspect the repository thoroughly.
2. Identify existing firmware, board definitions, pin mappings, drivers, protocol code, examples, build files, and documentation.
3. Do not invent hardware capabilities that are not supported by the actual repository/hardware.
4. Preserve existing functionality unless there is a clear reason to change it.
5. Prefer clean interfaces and stable abstractions over shortcuts.
6. Make assumptions explicit in documentation.
7. Add tests or validation wherever practical.
8. End each phase with a summary of what changed, what remains, and any architectural risks.

The implementation should be modular, versionable, debuggable, and understandable to another engineer.

---

# STEP 1 — REVERSE ENGINEER THE EXISTING MELO SYSTEM AND PRODUCE THE ARCHITECTURE CONTRACT

Prompt:

> First, inspect the entire repository and reverse engineer the current Melo hardware/software stack. Do not make broad implementation changes yet.
>
> Identify:
> - ESP platform and firmware architecture
> - how Arduino code currently runs, if applicable
> - pin mappings
> - motors and motor drivers
> - servos
> - sensors
> - LEDs
> - buzzer
> - serial/USB interfaces
> - existing communication/protocol mechanisms
> - flashing/upload workflow
> - device detection behavior
> - firmware versioning
> - existing APIs/classes
> - existing examples
> - web/block integration if present
> - build systems and dependencies
>
> Then create a concise technical architecture document in the repository that describes the current system and proposes the target Melo architecture:
>
> Hardware → ESP Firmware → Protocol/Transport → Melo SDK → Arduino / Melo Blocks.
>
> Clearly separate what already exists from what needs to be built.
>
> Do not invent missing facts. Where the repository is ambiguous, flag the ambiguity and state the smallest assumption needed to proceed.
>
> Also identify the smallest end-to-end vertical slice that can prove the architecture, preferably device detection + initialization + one motor command.
>
> Deliverables:
> - architecture document
> - hardware capability inventory
> - dependency/risk list
> - recommended implementation order
> - no large refactor yet unless required to understand the system.

---

# STEP 2 — DESIGN AND IMPLEMENT THE MELO SDK + PUBLIC API

Prompt:

> Based on the architecture and actual hardware discovered in Step 1, design and implement the Melo Arduino Library / SDK.
>
> Treat the SDK as the primary developer-facing abstraction layer for the robot.
>
> Build a clean public API around actual Melo capabilities. Where appropriate, support both:
>
> High-level robotics APIs such as:
> - Melo.begin()
> - Melo.moveForward(speed)
> - Melo.moveBackward(speed)
> - Melo.turnLeft(...)
> - Melo.turnRight(...)
> - Melo.stop()
>
> And component APIs such as:
> - motors
> - servos
> - sensors
> - LEDs
> - buzzer
> - system/device status
>
> Requirements:
> - hide low-level implementation details from normal users
> - keep advanced functionality extensible
> - isolate transport/protocol code from public API code
> - define consistent parameter ranges and units
> - define return values and error semantics
> - keep APIs intuitive for beginners
> - avoid hard-coding assumptions that should belong to firmware/configuration
>
> Implement the minimum complete vertical slice first, then expand component support.
>
> Add Arduino-compatible examples for every implemented capability.
>
> Add library metadata and installation documentation.
>
> Before finishing, compile/build all available examples and fix integration issues.

---

# STEP 3 — FORMALIZE THE SDK ↔ ESP PROTOCOL, DEVICE DETECTION, VERSIONING, AND ERROR HANDLING

Prompt:

> Now harden the boundary between the Melo SDK and ESP firmware.
>
> Inspect the existing communication mechanism and formalize a stable protocol contract.
>
> Define:
> - command identifiers
> - argument types and ranges
> - units
> - response formats
> - acknowledgement semantics
> - timeout behavior
> - retry behavior where appropriate
> - error codes
> - protocol versioning
> - hardware version
> - firmware version
> - device identification
> - compatibility checks
>
> Implement device discovery/handshake so the SDK can determine what Melo device it is communicating with and whether it is compatible.
>
> Implement meaningful error handling for:
> - device missing
> - connection failure
> - timeout
> - invalid command
> - invalid argument
> - firmware mismatch
> - protocol mismatch
> - sensor failure
> - calibration failure
> - hardware/transport failure
>
> The beginner-facing experience should produce actionable messages, not opaque codes alone.
>
> Keep protocol details separate from the high-level API so the public Melo API can remain stable even if the transport changes later.
>
> Add protocol documentation and tests for encoding/decoding and failure cases.

---

# STEP 4 — BUILD THE DEVELOPER TOOLING EXPERIENCE: FLASHING, SERIAL, DIAGNOSTICS, AND FIRMWARE UPDATE FOUNDATIONS

Prompt:

> Extend the Melo developer experience beyond the library.
>
> Based on the repository's actual environment, implement or structure the foundations required for:
> - device detection
> - connection state
> - firmware version display
> - firmware flashing/upload
> - firmware update checks
> - serial monitor/log viewing
> - diagnostic commands
> - connection troubleshooting
>
> Do not assume Melo Studio/Melo Lab is a specific desktop or web architecture unless the repository already establishes one. First identify the existing toolchain and implement the highest-value capabilities within it.
>
> The intended user journey is:
>
> Plug in Melo → detected → identify device → verify firmware → connect → upload/run → debug if needed.
>
> Make failures recoverable and explain them clearly.
>
> Keep flashing/update mechanisms version-aware and safe.
>
> Add developer documentation for the complete setup and debugging workflow.

---

# STEP 5 — CREATE THE SHARED CAPABILITY MODEL FOR MELO BLOCKS / WEB

Prompt:

> Treat the Arduino SDK and Melo Blocks as two interfaces to the same robotics capability model.
>
> Extract the semantic model of Melo hardware capabilities from the SDK. For each capability define:
> - stable capability name
> - inputs
> - types
> - valid ranges
> - units
> - outputs
> - side effects
> - errors
> - firmware/protocol requirements
>
> Examples may include:
> - MOVE_FORWARD
> - MOVE_BACKWARD
> - TURN_LEFT
> - TURN_RIGHT
> - STOP
> - SET_MOTOR
> - SET_SERVO
> - READ_DISTANCE
> - READ_SENSOR
> - SET_LED
> - BEEP
>
> Build this in a form that Melo Blocks can consume without directly depending on C++ implementation details.
>
> Ensure that a block such as:
>
> [MOVE FORWARD] [70]
>
> corresponds semantically to:
>
> Melo.moveForward(70);
>
> Avoid duplicating business logic between the SDK and web platform.
>
> Document the contract clearly and add validation so new capabilities cannot silently diverge across interfaces.

---

# STEP 6 — HARDEN, TEST, DOCUMENT, AND PREPARE FOR REAL USERS

Prompt:

> Perform a final engineering pass across the entire Melo stack.
>
> Verify:
> - clean repository structure
> - public API consistency
> - compile/build success
> - example programs work
> - protocol tests pass
> - device discovery works
> - version compatibility works
> - errors are understandable
> - documentation matches actual code
> - no stale APIs remain
> - no hidden magic constants are unnecessarily exposed
> - transport and hardware details are cleanly separated
> - shared capability definitions match Melo Blocks expectations
>
> Add or improve:
> - unit tests
> - integration tests where possible
> - smoke tests
> - example sketches
> - troubleshooting guide
> - API reference
> - architecture documentation
> - release/versioning notes
>
> Then produce a final engineering readiness report with:
> - implemented features
> - supported hardware capabilities
> - known limitations
> - unresolved assumptions
> - technical debt
> - recommended next milestones
> - exact commands needed to build/test/run the stack.

---

# OPTIONAL — ONE-SHOT MASTER PROMPT

Use this when you want Codex or Claude Code to own the entire task rather than running six separate phases.

> Take ownership of implementing the Melo developer stack from the current repository.
>
> Goal: make an ESP-based Melo robotics kit feel like a programmable robot rather than an ESP development board.
>
> First reverse engineer the repository and actual hardware. Then design and implement a layered system:
>
> Hardware → ESP Firmware → Protocol/Transport → Melo SDK → Arduino / Melo Blocks.
>
> Build the Melo Arduino Library / SDK with intuitive robot-level APIs for all real hardware capabilities, while keeping component-level APIs available for advanced users. Implement device initialization, discovery, compatibility checks, calibration where appropriate, error handling, examples, and documentation.
>
> Formalize the SDK ↔ firmware protocol, including commands, arguments, responses, errors, timeouts, versioning, device identity, and compatibility behavior. Keep the protocol separate from public API semantics.
>
> Build or structure the developer-tooling layer needed for device detection, flashing, firmware updates, serial monitoring, and diagnostics based on the actual repository/toolchain.
>
> Create a shared hardware capability model so Melo Blocks and the Arduino SDK represent the same concepts instead of duplicating hardware semantics.
>
> Work iteratively: prove a small end-to-end vertical slice first, then expand capabilities.
>
> Requirements:
> - inspect before editing
> - do not invent hardware
> - preserve working behavior
> - prefer modular architecture
> - document assumptions
> - add tests and examples
> - compile/build frequently
> - keep public APIs intuitive and stable
> - finish with a readiness report and explicit remaining work.

---

# HOW TO USE THESE PROMPTS

Recommended workflow:

```text
Step 1 → run once
Step 2 → implement SDK
Step 3 → stabilize protocol
Step 4 → developer tooling
Step 5 → Blocks/web contract
Step 6 → hardening/release
```

For a new or poorly understood repository, do not skip Step 1.

For a mature repository where the architecture is already known, Steps 2–5 can be executed sequentially with code review between phases.

The most important rule is:

> Do not let the coding agent build a large API surface before proving one complete hardware path end-to-end.
