# CubeSat Project

## System Architecture

        +---------------------------+
        |        Ground Station     |
        |        PC Application     |
        +-------------+-------------+
                    |
                WiFi / UART
                    |
                ESP32 COMMS
                    |
            Packet Protocol
                    |
                    UART
                    |

            STM32 Flight Computer (OBC)

            - Mission State Machine
            - Command Handler
            - Data Logger
            - Scheduler
            - Watchdog
            - Health Monitoring
            - Packet Router

                    |
                    SPI
                    |

            STM32 Housekeeping MCU

            - Temperature
            - Light
            - Voltage
            - Current
            - IMU
            - RTC
            - Sensor Drivers

### STM32 n1

Responsabilities:

- Scheduler
- Mission logic
- Command execution
- Flash logging
- Packet routing
- Watchdog supervision

Tasks:

- Telemetry Task
- Command Task
- Health Monitor
- Logger Task
- Mission Manager
- Heartbeat Task
- Watchdog Task

### STM32 n2

Responsabilities: (Only answer requests from Flight Computer)

- Sensor acquisition
- ADC
- IMU
- Temperature
- Light
- Battery monitoring

Tasks:

- Sensor Task
- ADC Task
- Power Monitor
- Telemetry Response Task

### ESP32

Responsabilities: (Communication Processor)

- Wifi
- Packet protocol

Tasks:

- Wifi Task
- UART Bridge
- Packet Parser
- Ground Station Server

## Communication

Packet protocol:
Header
0xAA55
SOURCE
DESTINATION
MESSAGE TYPE
LENGTH
PAYLOAD
CRC16

Commands:
GET_STATUS
GET_SENSOR_DATA
RESET_NODE
PING
START_LOGGING
SET_PARAMETER
GET_LOG_PAGE

## Software Engineering

1. Mile 1 (Bring up)

- UART
- GPIO
- LED
- SPI
- I2C

2. Mile 2 (drivers)

- IMU
- RTC
- Flash
- ADC

3. Mile 3 (packet protocol)

- CRC
- Parser
- Serializer

4. Mile 4 (communication)

- STM32 <-> STM32

5. Mile 5 (communication)

- STM32 <-> ESP32 (ground station)

6. Mile 6 (telemetry)

- Periodic packets (temp, voltage, current, orientation, mission time, heap usage, cpu load)

7. Mile 7 (commands)

- Reset
- LED
- Sampling Rate
- Read parameters
- Write parameters
- Force Sensor read
- Start logging

8. Mile 8 (logging)

- timestamp
- sensor values
- events
- errors

## Relieability

Implementing this will improve considerably the use of the project.

- Watchdogs
- CRC
- Heartbeat packets
- Timeouts
- Error codes
- Fault recovery
- Brownout detection
- Boot reason logging
- Safe mode

Eg: MCU stops responding (hearbeat lost -> retry -> reset mcu -> if still dead -> enter safe mode -> notify ground station)

## Ground station

Building a desktop app using Python + Tkinter/Pyside6
Features:

- Live telemetry
- Mission timer
- Command buttons
- Packet viewer
- Event log
- Sensor plot
- Firmware version
- Health monitor

## Testing

- Unit tests
- Packet tests
- CRC tests
- Parser tests
- Stress tests
- Communication tests

Also including this results with plots and logs in the documentation

- 24-hour runtime
- Memory leak test
- Packet corruption test
- Watchdog test
- Power cycling
- Disconnect tests

## Documentation

- Architecture diagrams showing the 3 MCU topology and data flow
- Protocol specification with packet formats, command IDs, CRC details and timing
- State machine diagrams for boot, nominal operation, safe mode, and fault recovery
- Interface control document (ICD) defining the communication between each subsystem
- Requirements traceability linking high-level requirements to implement features and tests
- Test reports with procedures, results and observed limitations
