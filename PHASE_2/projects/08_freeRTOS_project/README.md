# Project Statement

In aerospace systems, spacecraft and aircraft continuously monitor **environmental conditions, structural loads, and system health**. These measurements are processed in real-time by embedded computers which must handle multiple tasks simultaneously while ensuring **deterministic timing and safe data sharing**.

In this project, you will design and implement a **real-time telemetry and health monitoring system** for a **simulated spacecraft payload module** using an **ESP32 running FreeRTOS**.

The system will acquire data from multiple sensors, process the data concurrently, detect abnormal conditions, and transmit telemetry packets to a ground station.

The goal of this project is to **apply all major FreeRTOS synchronization and communication mechanisms** in a realistic embedded system scenario.

---

# System Overview

Your system represents the **Onboard Data Handling (OBDH) subsystem** of a spacecraft payload.

It must perform the following functions:

### 1. Sensor Acquisition

The system reads multiple sensors periodically:

Examples (choose available sensors):

- **Temperature sensor** (thermal monitoring)
- **IMU / accelerometer** (vibration monitoring)
- **Barometric pressure sensor** (altitude simulation)
- **Light sensor** (sun exposure)
- **Voltage/current sensor** (power monitoring)

Sensor readings must be acquired at **different sampling rates**.

Example frequencies:

| Sensor           | Frequency |
| ---------------- | --------- |
| Temperature      | 1 Hz      |
| IMU              | 50 Hz     |
| Pressure         | 5 Hz      |
| Power monitoring | 2 Hz      |

---

### 2. Data Processing

The system must:

- Filter raw sensor data (moving average or similar)
- Detect anomalies (out-of-range values)
- Package telemetry messages

---

### 3. Telemetry Transmission

Telemetry packets must be transmitted to a **ground station** via:

- Serial interface
- WiFi
- Bluetooth

Telemetry messages should include:

```
timestamp
sensor id
sensor value
system status
```

Transmission rate: **1 Hz**

---

### 4. Fault Detection

The system must detect abnormal situations such as:

- Overtemperature
- Excessive vibration
- Power voltage drop
- Sensor malfunction

When a fault is detected:

- A **warning flag** must be raised
- The telemetry packet must include the fault status
- A **LED indicator must blink**

---

# FreeRTOS Requirements

Your implementation **must include and demonstrate the use of all the following FreeRTOS mechanisms.**

---

# Required Task Architecture

Design at least the following tasks:

| Task               | Priority | Description                           |
| ------------------ | -------- | ------------------------------------- |
| Sensor Acquisition | Medium   | Reads sensors periodically            |
| Data Processing    | Medium   | Filters and analyzes sensor data      |
| Telemetry Task     | Low      | Sends data to ground station          |
| Fault Monitor      | High     | Detects anomalies and triggers alerts |
| Debug/Logger Task  | Low      | Prints debug messages                 |

You may add additional tasks if needed.

---

# Required FreeRTOS Mechanisms

## 1. Task Priorities

You must justify the priority selection.

Example concept:

```
High Priority → Fault monitoring
Medium Priority → Sensor acquisition
Low Priority → Telemetry transmission
```

Explain in your report why these priorities are chosen.

---

# 2. Queues (Inter-task Communication)

Use **FreeRTOS queues** to send sensor data between tasks.

Example architecture:

```
Sensor Task → Queue → Processing Task
Processing Task → Queue → Telemetry Task
```

Queue messages should contain a structure similar to:

```
typedef struct
{
    uint32_t timestamp;
    uint8_t sensor_id;
    float value;
} sensor_data_t;
```

---

# 3. Mutex

Use a **mutex** to protect a shared resource.

Example resources:

- Serial port
- Shared sensor data buffer
- Telemetry packet buffer

Example scenario:

Multiple tasks may attempt to **print debug information simultaneously**.

---

# 4. Binary Semaphore

Use a **binary semaphore** for **interrupt-to-task synchronization**.

Example:

A hardware interrupt from a sensor signals that new data is ready.

```
ISR → gives semaphore
Sensor task → waits for semaphore
```

Possible implementations:

- IMU data ready interrupt
- Button triggering emergency telemetry

---

# 5. Counting Semaphore

Use a **counting semaphore** for managing **multiple data events**.

Example:

Sensor task produces samples faster than the processing task consumes them.

The counting semaphore tracks **available data samples**.

---

# 6. Event Groups

Use **Event Groups** to represent system states.

Example flags:

```
BIT0 → Temperature warning
BIT1 → Vibration warning
BIT2 → Power warning
BIT3 → Sensor failure
```

The fault monitoring task sets event bits.

The telemetry task reads these bits to include status in telemetry packets.

---

# 7. Software Timers

Implement at least **two FreeRTOS software timers**.

Examples:

### Telemetry Timer

Triggers telemetry transmission every **1 second**.

### Health Check Timer

Runs periodic system checks every **5 seconds**.

Timers must execute **callback functions**.

---

# 8. Task Notifications

Use **direct task notifications** for fast communication between tasks.

Example:

```
Sensor Task → notifies Processing Task
```

This should be used instead of queues for **simple event signals**.

---

# 9. Debugging and Monitoring

Your system must include debugging features.

Examples:

- Print task states
- Print queue status
- Monitor stack usage
- Detect queue overflow

Use functions such as:

```
uxTaskGetStackHighWaterMark()
vTaskList()
xQueueSpacesAvailable()
```

The **Logger Task** should periodically output system diagnostics.

---

# Suggested Task Diagram

```
                +--------------------+
                | Sensor Tasks       |
                | (multiple)         |
                +---------+----------+
                          |
                          | Queue
                          v
                +--------------------+
                | Data Processing    |
                +---------+----------+
                          |
                          | Queue
                          v
                +--------------------+
                | Telemetry Task     |
                +---------+----------+
                          |
                          v
                   Ground Station

Fault Task ----> Event Groups ----> Telemetry Task

ISR ----> Semaphore ----> Sensor Task
```

---

# Minimum Deliverables

Students must submit:

### 1. Source Code

Complete ESP32 project.

---

### 2. Architecture Diagram

Diagram showing:

- Tasks
- Queues
- Semaphores
- Event groups
- Timers

---

### 3. Technical Report (5–10 pages)

Include:

1. System architecture
2. Task design
3. Explanation of FreeRTOS mechanisms used
4. Justification of task priorities
5. Debugging results
6. Performance observations

---

### 4. Demonstration

During the lab demonstration you must show:

- Telemetry messages
- Fault detection triggering
- Debug output
- Real-time task behavior

---

# Bonus Challenges (Optional)

Students seeking extra credit may implement:

### 1. Telemetry Packet Protocol

Design a structured binary telemetry packet similar to spacecraft telemetry.

---

### 2. Watchdog Task

Implement a watchdog that resets the system if tasks stop responding.

---

### 3. Data Logging

Store telemetry data on:

- SD card
- SPIFFS

---

### 4. Ground Station Visualization

Create a Python script that plots incoming telemetry data in real time.

---

# Evaluation Criteria

| Category                 | Weight |
| ------------------------ | ------ |
| System Architecture      | 20%    |
| Correct FreeRTOS Usage   | 35%    |
| Code Quality             | 15%    |
| Debugging Implementation | 15%    |
| Documentation            | 15%    |

---

# Expected Learning Outcomes

By completing this project you will learn to:

- Design **multi-tasking embedded systems**
- Use **real-time synchronization mechanisms**
- Manage **inter-task communication**
- Debug **complex RTOS-based systems**
- Build **aerospace-style telemetry software**
