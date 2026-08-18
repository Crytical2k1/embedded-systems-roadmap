STM32 x2
ESP32
IMU
LightSensors
Temperature sensors
Volgate Sensors
RTC
microSD Card module
Buzzer
Status LEDs

## Block diagram

                    +---------------------+
                    |     Ground PC       |
                    | Python Ground App   |
                    +----------+----------+
                               |
                           WiFi / USB
                               |
                        ESP32 Communication
                               |
                             UART
                               |

+-------------------------------------------------------------+
| STM32 Flight Computer (OBC) |
|-------------------------------------------------------------|
| Mission Manager |
| Scheduler |
| Data Logger |
| Packet Router |
| Watchdog |
+--------------------------+----------------------------------+
|
SPI
|
+-------------------------------------------------------------+
| STM32 Housekeeping |
|-------------------------------------------------------------|
| IMU |
| Temperature |
| Light |
| Voltage |
| Current |
| RTC |
| Environmental Sensors |
+-------------------------------------------------------------+

## Wiring matrix

STM32 OBC - STM32 HK - SPI
STM32 OBC - ESP32 - UART
STM32 OBC - SD Card - ??
STM32 HK - IMU - I2C
STM32 HK - Light sensor - ADC
STM32 HK - Temperature - ADC
STM32 HK - Voltage - I2C
STM32 HK - RTC - I2C
