# System Requirements Specification (SRS)

Project: CubeSat Flight Computer Platform
Document ID: SRS-001
Version: 0.1
Status: Draft

## Introduction

### Purpose

This document defines the functional and non-functional requirements for the CubeSat Flight Computer Platform.
The purpose of the system is to simulate the avionics architecture of a CubeSat by implementing a modular embedded system composed of three cooperating microcontrollers.
The systems shall demonstrate professional embedded software engineering practices including:

- Real-time task scheduling
- Fault detection and recovery
- Telemetry generation
- Command handing
- Sensor acquisition
- Data logging
- Inter-processor communication

### Scope

The project is not intended to operate in space.
Instead, if provides a high-fidelity engineering demonstration of an embedded satellite computer suitable for portfolio and educational purposes.
The system shall emulate the architecture commonly found in CubeSat avionincs.

### Objectives

The project shall demonstrate:

- modular firmware architecture
- hardware abstraction
- RTOS design
- communication protocols
- robustness
- fault tolerance
- software verification

## System overview

1. Hardware (3 independent processing units)

   Grounds station -> WIFI/UART -> ESP32 -> UART -> STM32 -> SPI -> STM32

2. Flight Computer (OBC)

   Responsabilities:

- Mission management
- Telemetry management
- Command execution
- Fault supervision
- Logging
- Scheduling
- Communication routing

3. Housekeeping MCU

   Responsabilities:

- Sensor acquisition
- ADC
- IMU
- Environmental monitoring
- Power monitoring

4. ESP32

   Responsabilities:

- Communication bridge
- Ground station interface
- Packet forwarding

## Operational Concept

The system will operate continuously.
During operation:

- Sensors are sampled
- Sensor values are sent to OBC
- OBC updates system state
- OBC stores telemetry
- Telemetry is transmitted
- Ground station issues commands
- Commands are executed
- Results are reported

## Functional Requirements

Every requirement receives an identifier and should be measurable, testable and unambiguous.

Eg:
SYS-001
The system shall initialize all processing units withing five seconds after power-up.

### Boot Requirements

- SYS-001: The system shall boot within 5 seconds after power is applied
- SYS-002: Each MCU shall perform a Power-On-Self-Test (POST)
- SYS-003: The Flight Computer shall verify communication with every subsystem before entering Nominal Mode.
- SYS-004: If a subsystem does not respond withing 3 seconds, the Flight Computer shall enter Safe Mode.

### Communication Requirements

- COM-001: The Flight Computer shall communicate with the Housekeeping MCU
- COM-002: The Flight Computer shall communicate with the ESP32
- COM-003: Header - Source - Destination - Packet Type - Payload Length - Payload - CRC16
- COM-004: Corrupted packets shall be discarded
- COM-005: Packet corruption shall be detected using CRC16
- COM-006: Lost communication shall be detected within 2 heartbeat intervals
- COM-007: Maximum communication latency shall not exceed 100ms under nominal conditions

### Sensor Requirements

- SEN-001: The Housekeeping MCU shall sample every enabled sensor
- SEN-002: Sampling frequency shall be configurable
- SEN-003: Sensor failuers shall be reported
- SEN-004: Invalid sensor values shall never be forwared without an error flag

### Telemetry Requirements

- TEL-001: Telemetry shall be generated periodically
- TEL-002: Telemetry period shall be configurable
- TEL-003: Telemetry should include: - timestamp - temperature - voltage - current - IMU - system state - error flags - uptime
- TEL-004: Telemetry packets shall contain sequence numbers
- TEL-005: Missed packets shall be detectable

### Command Requirements

- CMD-001: The system shall accept commands from the Ground Station
- CMD-002: Only valid commands shall be executed
- CMD-003: Invalid commands shall return an error response
- CMD-004: Every command shall generate an acknowledgement
- CMD-005: Command execution shall be logged

### Logging Requirements

- LOG-001: Mission event shall be stored
- LOG-002: Faults shall be stored
- LOG-003: Boot events shall be stored
- LOG-004: Logs shall survive resets
- LOG-005: Logs shall be downloadable

### Fault Management

- FMS-001: Every subsystem shall periodically transmit heartbeat messages
- FMS-002: Heartbeat timeout shall trigger subsystem recovery
- FMS-003: If recovery fails, Safe Mode shall be entered
- FMS-004: Faults shall be reported to the Ground Station
- FMS-005: Watchdog resets shall be looged

### Time Management

- TIM-001: The Flight Computer shall maintain Mission Elapsed Time (MET)
- TIM-002: Every telemetry packet shall include MET
- TIM-003: System clocks shall remain synchronized to within +-10ms under nominal operation

## Non-Functional Requirements

This define how well the system performs rather than what it does.

### Reliability

- REL-001: The system shall operate continuously for at least 24 hours without requiring a reboot
- REL-002: No memory leaks shall be observed during continuous operation
- REL-003: Unexpected resets shall be detected and logged

### Performance

- PER-001: CPU utilization on the Flight Computer shall remain below 70% during nominal operation
- PER-002: Sensor acquisition jitter shall remain below 5ms
- PER-003: Telemetry latency shall remain below 200ms

### Maintainability

- MAI-001: All hardware access shall be encapsulated by a Hardware Abstraction Layer (HAL)
- MAI-002: Application logic shall not directly access MCU peripheral registers
- MAI-003: Each module shall expose a documented public interface and minimize coupling with other modules

### Portability

- POR-001: Application code shall be independent of MCU-specific register definitions whenever practical
- POR-002: Communication modules shall be transport-independent (eg., capable of using UART or CAN with minimal changes)

### Safety

- SAF-001: Watchdogs shall be enable during nominal operation
- SAF-002: Invalid commands shall not modify system state
- SAF-003: The system shall enter Safe Mode after repeated unrecoverable communication failures

## System States

Define the high-level operational modes

- Boot - Hardware initialization and POST
- Initialization - Establish communication and configure subsystems
- Nominal - Normal mission execution, telemetry and command processing
- Safe Mode - Minimal functionality after critical fault detection
- Fault Recovery - Attempt automatic recovery of failed subsystems
- Shutdown - Controlled stop for maintenance or simulation end

## Verification Matrix

- SYS-001: Measure boot time with logic analyzer or timestamp logging
- COM-004: Inject packets with invalid CRC and verify rejection
- SEN-002: Change sampling rate via command and measure intervals
- TEL-003: Compare transmitted telemetry against sensor values
- CMD-004: Send commands and verify acknowledgement
- LOG-004: Reset the system and confirm log persistence
- FMS-002: Disconnect Housekeeping MCU and verify recovery attermp
- REL-001: Execute a 24-hour endurance test with continuous telemetry
