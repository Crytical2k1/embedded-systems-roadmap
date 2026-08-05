# Software Architecture Document (SAD)

Project: CubeSat Flight Computer Platform
Document ID: SAD-001
Version: 0.1
Status: Draft

## Purpose

This document defines the internal software architecture of the CubeSat Flight Computer Platform.
It specifies:

- Software layers
- Module responsibilities
- Task organization
- Inter-module communication
- Memory ownership
- Execution model
- Coding conventions
- Dependency rules

## Design Principles

### Single Responsability

Each software module shall have one well-defined responsability.
Eg,

- Logger logs
- Sensor manager acquires sensors
- Packet parser parses packets
- Scheduler schedules tasks

### Dependency Direction

Dependencies shall always point downward, lower layers shall never call higher layers.

        Application
            |
        Services
            |
        Drivers
            |
        HAL
            |
        Hardware

### Hardware Independence

Application code shall never access STM32 HAL directly.

Forbidden:
HAL_UART_Transmit(...)
Allowed:
Telemetry_Send(...) / Serial_Write(...)

Making migration easier.

### Event Driven

Tasks should react to:

- events
- queues
- timers
- notifications

Avoid polling whenever practical.

### No Global Variables

Except for

- RTOS handles
- Configuration table
- Immutable lookup tables

Everything else belongs to a module.

## Software Stack

Each layer communicates only with the one immidiately below it.

| Stack                          |
| ------------------------------ |
| Mission Application            |
| Platform Services / Middleware |
| Device Driver Layer            |
| Board Support Package (BSP)    |
| STM32 HAL / ESP-IDF / CMSIS    |
| Hardware                       |

## Architecture Per MCU

1. Flight Computer

The OBC owns all system decisions

- Mission Manager
- Telemetry Manager
- Command Manager
- Fault Manager
- Logger
- Parameter Manager
- Time Manager
- Packet Router
- Scheduler
- Watchdog

2. Housekeeping MCU

No mission logic

- Sensor Manager
- Power Monitor
- ADC Driver
- IMU Driver
- RTC Driver
- Health Monitor
- Packet Handler

3. ESP32

No control logic

- WIFI Server
- UART Bridge
- Packet Router
- OTA Service
- Diagnostics

## Directory Structure

Each firmware shares the "common" library:

firmware/
|- common/
| |- protocol/
| |- crc/
| |- ringbuffer/
| |- queue/
| |- parameter/
| |- utilities/
|
|- obc/
|- hk/
|- esp32/
|- tests/

## Internal Layering

This structure prevents application code from depending on vendor libraries, each MCU includes:

- Application Layer:
  Mission
  Telemetry
  Commands
- Services:
  Logger
  Time
  Packet
  Parameters
  Fault
- Drivers:
  IMU
  UART
  ADC
  SPI

- HAL

## Flight Computer Modules

1. Mission Manager

- Mission state
- State transitions
- Safe Mode
- Startup sequence

2. Telemetry Manager

- Packet generation
- Scheduling telemetry
- Prioritization
- Buffering

3. Command Manager

- Decoding commands
- Autorization
- Dispatching
- Acknowledgements

4. Logger

- Persistent storage
- Event logging
- Fault logging
- Telemetry history

5. Fault Manager

- Heartbeat monitoring
- Watchdog handing
- Recovery procedures
- Safe Mode

6. Parameter Manager

- Persisten configuration
- Parameter validation
- Default values
- Versioning

7. Time Manager

- Mission Elapsed Time (MET)
- Synchronization
- Software timers
- Timestamps

8. Packet Router

- Routing packets
- Forwarding
- Destination lookup

## Housekeeping Modules

Acting as data acquisition subsystem

- Sensor Scheduler
- Driver Layer
- Measurement Filter
- Telemetry Builder
- Packet Handler

## ESP32 Modules

The ESP32 should be stateless regarding the mission

- TCP server
- Packet parser
- UART bridge
- Diagnostics

## RTOS Architecture

Each major responsability is implemented as a dedicated FreeRTOS task.
Design note: Avoid assigning periodic tasks directly to RTOS delays (vTaskDelay()) for time-critical behaviour. Prefer software timers, task notification, or vTaskDelayUntil() to minimize jitter.

1. OBC
   |Task|Priority|Period/Event|
   |--|--|--|
   |Mission Task | High | Event-driven|
   |Command Task | High|Queue|
   |Telemetry Task | Medium| 1Hz timer|
   |Fault monitor | High | 100ms|
   |Logger Task | Low | Queue|
   |Packet router | High | Queue|
   |Time Manager| High| 1ms tick|
   | Watchdog service | Highest | 50ms|
2. HK
   |Task|Priority|
   |--|--|
   |Sensor Acquisition | High|
   |ADC Sampling | High|
   |Packet Handler | Medium|
   |Health Monitor| Medium|
3. ESP32
   |Task|Priority|
   |--|--|
   |Wifi Server | Medium|
   |UART bridge | High|
   |Packet router | High|
   |Diagnostics | Low|

## Inter-Task Communication

No task shall directly manipulate another task's internal state.
Use RTOS primitives:

- Queues for packet and commands
- Message buffers for variable-length telemetry
- Stream buffers for serial byte streams
- Task notifications for lightweight events
- Event groups for system-wide status flags
- Mutexes only to protect shared resources (eg, flash storage)
  (Eg, UART ISR - Stream Buffer - Packet Parser Task - Command Queue - Command Manager - Response Queue - Telemetry Task - UART Driver)
  This separates interrupt handling form application logic and keeps ISRs short.

## Memory Ownership

Each modules owns its data.
Eg, Telemetry Manager owns TelemetryBuffer
No other module writes to it and request happen through APIs.

## Error Handling

Errors never disappear.
Module -> Error Manager -> Logger -> Ground Station

Every error receives:

- Timestamp
- Source module
- Severity
- Error code

Defining severity levels:
|Level|Meaning|
|----|----|
|INFO | Infomational events|
|WARNING | Recoverable anomaly|
|ERROR |Functional failuer|
|CRITICAL | Immediate transition to Safe Mode|

## Coding Standards

- C17 (or C11 if toolchain limitations require it)
- snake_case for functions and variables
- PascalCase for structs and enums
- Fixed-width integer types (uint32_t, etc)
- No dynamic memory allocation after initialization
- One module per responsability
- Public API in .h, implementation in .c
- Every module documented
  (Considering using rules with clang-format and clang-tidy to keep the codebase consistent)

## Configuration Management

Configuration is centralized in a Parameter Database

Eg, Telemetry Rate, Heartbeat Rate, Sensor Enable, Sampling Frequency, Log Level, Network Port...

Each parameter receives:

- Unique ID
- Type
- Default value
- Limits
- Persistence flag

## Testing Strategy

Every module shall be independently testable.
Eg, Packet Parser -> Unit Test (No UART required)
Eg, CRC -> Unit Test (No hardware required)
The goal is to maximize host-based testing before deploying to the target hardware

## Build System

Support multiple build targets:
Target (STM32, ESP32, Host PC)

This enables compiling shared libraries (CRC, protocol, parameter database, packet parser, state machines) or a desktop for automated unit tests
Possible layout
firmware/
tests/
cmake/
docs/

Using CMake as the top-level build system allows integration with STM32CubeIDE, ESP-IDF, and host-side test frameworks.
