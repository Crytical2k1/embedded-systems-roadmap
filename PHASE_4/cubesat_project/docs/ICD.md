# Interface Control Document (ICD)

Project: CubeSat Flight Computer Platform
Document ID: ICD-001
Version: 0.1
Status: Draft

## Purpose

This document specifies every electrical, logical and software interface between the subsystems of the CubeSat platform.

- Define communication protocols
- Specify packet formats
- Assign subsystem responsabilities
- Standardize timing requirements
- Define command and telementry interfaces
- Enable independent firmware development

## System Interfaces

The project contains four logical nodes.

- Fligth Computer - OBC - Main controller
- Housekeeping - HK - Sensor acquisition
- Communication - COM - ESP32 Communication Bridge
- Ground Station - GS - Desktop application

        Ground Station
             |
        Wifi / UART
             |
         ESP32 COM
             |
            UART
             |
      Flight Computer (OBC)
             |
            SPI
             |
      Housekeeping (HK)

## Physical Interfaces

1. OBC <-> HK

- Interface - SPI
- Baud Rate - 115200 bps
- Data bits - 8
- Stop bits - 1
- Parity - None
- Flow Control - None

2. OBC <-> ESP32

- Interface - UART
- Baud Rate - 115200 bps
- Data bits - 8
- Stop bits - 1
- Parity - None
- Flow Control - None

3. ESP32 <-> Ground Station

- Interface - WIFI
- Port - 5000

## Node Responsabilities

1. Flight Computer
   Owner of

- Mission state
- Scheduler
- Logging
- Fault management
- Command routing
- Configuration database
- Time synchronization
  Never
- Reads sensors directly
- Talks to ground station directly

2. Housekeeping
   Owner of

- Sensor drivers
- ADC
- IMU
- Environmental sensors
  Never
- Execute mission logic

3. ESP32
   Owner of

- WIFI
- Packet forwarding
- TCP server
- Serial bridge
  Never
- Stores mission state

## Communication Model

Every packet follows this flow:
Sender
|
Packet
|
Receiver
|
CRC Verification
|
Packet Parsing
|
Command Dispatch
|
Response
|
ACK

## Packet Format

Every subsystem uses exactly the same packet structure

| Byte    | Field            | Size     |
| ------- | ---------------- | -------- |
| 0       | Sync Word 1      | 1        |
| 1       | Sync Word 2      | 1        |
| 2       | Protocol Version | 1        |
| 3       | Source           | 1        |
| 4       | Destination      | 1        |
| 5       | Message Type     | 1        |
| 6       | Flags            | 1        |
| 7-8     | Sequence Number  | 2        |
| 9-10    | Payload Length   | 2        |
| 11..N   | Payload          | Variable |
| N+1.N+2 | CRC16            | 2        |

## Node IDs

| Device          | ID   |
| --------------- | ---- |
| Broadcast       | 0x00 |
| Ground Station  | 0x01 |
| ESP32           | 0x02 |
| Flight Computer | 0x03 |
| Housekeeping    | 0x04 |

## Message Types

| Type            | Value |
| --------------- | ----- |
| Hearbeat        | 0x01  |
| Command         | 0x02  |
| Command ACK     | 0x03  |
| Telemetry       | 0x04  |
| Event Log       | 0x05  |
| Error           | 0x06  |
| Parameter Read  | 0x07  |
| Parameter Write | 0x08  |
| Time Sync       | 0x09  |
| Boot Report     | 0x0A  |

## Command IDs

| Command           | ID    |
| ----------------- | ----- |
| Ping              | 0x001 |
| Reset MCU         | 0x002 |
| Request Telemetry | 0x003 |
| Read Sensor       | 0x004 |
| Set Sample Rate   | 0x005 |
| Enable Sensor     | 0x006 |
| Disable Sensor    | 0x007 |
| Start Logging     | 0x008 |
| Stop Logging      | 0x009 |
| Read Log          | 0x00A |
| Erase Log         | 0x00B |
| Read Parameter    | 0x00C |
| Write Parameter   | 0x00D |
| Enter Safe Mode   | 0x00E |

## Telemetry Packet

Telemetry should remain fixed-length for efficiency parsing and version compatibility. If new fields are required, define a new telemetry version or use optional extension packes.
Every telemetry frame contains:

| Field        | Size |
| ------------ | ---- |
| Mission Time | 1B   |
| System State | 1B   |
| CPU Load     | 1B   |
| Heap Free    | 2B   |
| Temperature  | 2B   |
| IMU X        | 2B   |
| IMU Y        | 2B   |
| IMU Z        | 2B   |
| Error Flags  | 4B   |

## Hearbeat

Every node transmit the "Heartbeat" once every 1000ms.

| Field         | Size |
| ------------- | ---- |
| Node ID       | 1    |
| System State  | 1    |
| Reset Counter | 1    |
| Uptime        | 4    |

If 3 missed heartbeats then the Node becomes Offline.

## Error Codes

| Code   | Meaning         |
| ------ | --------------- |
| 0x0000 | OK              |
| 0x0001 | CRC Error       |
| 0x0002 | Invalid Command |
| 0x0003 | Invalid Length  |
| 0x0004 | Sensor Failure  |
| 0x0005 | Timeout         |
| 0x0006 | Memory Full     |
| 0x0007 | Flash Failure   |
| 0x0008 | Watchdog Reset  |
| 0x0009 | Unknown Error   |

## State Synchronization

Only OBC owns the mission state, Housekeeping or ESP32 cant change mission mode.
Eg, GS -> ESP32 -> OBC -> Mission State Update -> Broadcast New State -> HK Updates Internal Status

Single ownership prevents conflicting state changes and simplifies fault analysis.

## Timing Requirements

| Event             | Requirement   |
| ----------------- | ------------- |
| Hearbeat          | 1000ms        |
| Sensor Sampling   | 100ms default |
| Telemetry         | 1000ms        |
| Command ACK       | <100ms        |
| Command Execution | <500ms        |
| Boot Report       | Within 2s     |
| Recovery Retry    | 5s            |

## Fault Handling

1. Communication timeout
   Retry x3 -> Reset Interface -> Reset Node -> Safe Mode -> Nofity GS
2. CRC failure
   Discard Packet -> Increment Counter -> Wait Next Packet
3. Invalid command
   Send Error Packet -> Continue Normal Operation

## Versioning

Each packet includes a protocol version: Version 1 0x01.
Future firmware must reject incompatibility versions while provinding an appropiate error response, allowing controlled protocol evolution.

## Reserved Fields

The packet header contains reserved bits for future capabilities:

- Packet priority
- Encryption indicator
- Compression indicator
- Fragmentation support
- Retransmission request
  Keeping these fields defined but unused helps preserve backward compatibility
