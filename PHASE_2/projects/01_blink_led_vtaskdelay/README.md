# Blinking LED using vTaskDelay

First project using an ESP32, and the goal is to get started with FreeRTOS. FreeRTOS is included by default in the ESP32. The requirements of the project is to blink an LED using vTaskDelay.

## Hardware

NodeMCU 32s with and ESP32
Built in led (gpio 2)

## Procedure

- Creation of the task, led_task, that initializes the led port and sets it as output; and includes the loop that toggles on an off the led with a delay of 500ms used inside the vTaskDelay() function, converting the ms to ticks, as the scheduler works by ticks not time itself. The benefit of using vTaskDelay is that is non blocking in difference to delay() used in arduino or non FreeRTOS, vTaskDelay gives the option to the scheduler to block the task if another with higher priority comes through.
- Defining the task in the app_main, for the scheduler to know that the task exists and its given its stack size and priorities.

## Improvements

- Modify the vTaskDelay to vTaskDelayUntil for a more precise timing
