# Light-Driven Motor Tracker

## Overview

This project implements a closed-loop light tracking system on the ESP32 using FreeRTOS. Two photoresistros detect the direction of the strongest light source, while an IMU provides heading feedback. An angle controller rotates the motor until the system is aligned with the light source. When no light is detected for a configurable amount of time, the system performs a full sweep to search for a new light source.

## Features

- Dual photoresistor light detection
- IMU-based heading stimation
- Closed-loop angle controller
- FreeRTOS multitasking
- Automatic light search mode
- State machine for tracking and recovery
- Modular hardware drivers

## Hardware

Component ----------- Purpose

ESP32 --------------- Main controller

DC motor ------------ Rotates the platform

Motor driver -------- Drives the motor

Encoder ------------- Motor feedback

IMU ----------------- Heading estimation

Photoresistos ------- Light detection

## Wiring

...

## Software Architecture

(Describe the modules angle_controller, cds_photoresistor...)

## Control Flow

          Left Sensor
               │
               │
               ▼
          Light Error
               ▲
               │
         Right Sensor

               │
               ▼
      Desired Heading
               │
               ▼
      Angle Controller
               │
               ▼
          Motor Driver
               │
               ▼
             Motor
               │
               ▼
         IMU Heading
               │
               └─────────────── Feedback

## State Machine

                 +---------+
                 |  IDLE   |
                 +---------+
                      │
              Light detected
                      │
                      ▼
               +-------------+
               |  TRACKING   |
               +-------------+
                      │
             No light timeout
                      │
                      ▼
               +-------------+
               |  SWEEPING   |
               +-------------+
                      │
          Sweep completed
                      │
                      ▼
               +--------------+
               | RETURNING    |
               +--------------+
                      │
              Back to start
                      │
                      ▼
                 +---------+
                 |  IDLE   |
                 +---------+

## Control Algorithm

1. Read both photoresistors
2. Compute the light error
3. Convert the error into a desired heading
4. Angle controller computes motor command
5. IMU provied heading feedback
6. Repeat every 100ms

## FreeRTOS Tasks

Photo Task ------------- 100ms -------------- Detect light and update target

IMU Task --------------- 10ms --------------- Update heading estimate

Angle Controler -------- 10ms --------------- Compute motor command
