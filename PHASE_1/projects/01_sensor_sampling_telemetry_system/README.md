# ADC data acquisition

This is the first embedded project to put into practice some of the knowledge i got from the theory of the phase 0, including bit operations, register and port designation, serial communication and sampling frecuency.

## Project idea

Generate code that will collect ADC data, convert it into digital and send it through serial communication. With an added feature of changing the sampling period at runtime.

## Hardware

Elegoo/Arduino Uno R3 (ATMega328P)
Light Sensor
Resistor 10kOhm

## Process of code development

1. Timer setup
   Initializing a timer (Timer1 16-bit) using the internal interrupt uses an ISR to increase a counter evey 1ms. The Timer1 gives more freedom at setup time as Timer0 is used by Arduino hardware.

2. GPIO setup
   LED port set up to output and leg toggle function.

3. Serial setup
   Initializes the serial by setting up the baud rate and enabling the transmision and recieve data.
   Stores the functions to write data in the serial, and functions to read the recieved data, creating a flag used to detect when data is available to be read.

4. ADC setup
   Initializes the analog-digital converter and handles the input conversion.

5. App setup
   As the project grew, the need of creating this file to store the logic and handling of the modules.
   Holds the logic that checks the time elapsed from last sample and runs the logic to get the data sensor and send it through serial.
   At the same times holds the logic to modify the sample period during runtime.
   - handle_serial_rx: Checks if a reading value is available (without blocking), checks when the command ends ('\n') and is protected of overflowing by discarding the command
   - handle_serial_commmand: checks the command start and calculates the new period value running through each value of the command sent. Finally checks if the new period value is inside defined values.
   - app_set_sampling_period: sets the new sampling period and resets the last_sample_time so that it doesnt generate doble read of sensor data

6. Main setup
   Initializes each module, timer, GPIO, serial, ADC and global interrupts. Inside the main loop, calls the serial read function to check if new sampling period is available and runs the sampling function.

## Problems encountered

Main issue was understanding how to handle the data read through serial and store the information to handle it.

Having to check when a command finishes, checking that the command starts with the corresponding value and running through the buffer values to calculate the corresponding new sampling period.
