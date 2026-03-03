# Queue Communication

The main goal of this project is to learn and put into practice the usage of queue management, we will create a task that reads data from an I2C sensor and sends a strucure with the sensor raw data and the timestamp to a "Controller" task.

## Hardware

NodeMCU 32s
IMU Module GY-521 MPU 6500

## Procedure

1. Set up the sensor
   The first step before doing any communication is to set up the sensor and check if it is reading data. As im using an gy521 the communication to read the data is done through I2C communication, set up with the driver i2c_master where we had to configure and initialize the bus and configure and initialize the device on the bus. Adding the wake up code of the sensor as it starts in sleep mode by writting to the power management register.
   Finally, the reading code includes the i2c read command to read the register of acceleration and a small loop to copy the data from the buffer into the acceleration variable in the proper format, as the buffer had 8bits, and the acceleration 16bits.

2. Create Tasks
   To keep it simple here, we created 2 tasks, one in charge of getting the data from the sensor, and another task (acting as a controller) that gets the data and prints the acceleration values, we could add here some angle calculations or something fancier but to test the i2c and queue communication is enough.

3. Link the tasks with the queue
   To adde the inter-task communication the queue was created with a size to hold the 3 values of acceleration and up to 5 values in the queue. Then inside each task (sensor and controller) the data send/receive code was added to communicate through the queue, both cases checking the status in case the infomation is lost, and finally the data recieved by the controller is printed on serial to confirm that the communication is happening.

## Issues

Starting the hardware connection, we connected the vcc input of the gy521 to the 3.3V output of the esp32, and while running i2c scans we couldnt find the address of the sensor indicating there were some issues with the communication, ending up being an issue of power, as the sensor needed 5V on the input to operate, more likely intensity needs not supplied by the esp32, but solved by powering the sensor with an external 5V (from an Arduino). Finding the address with no issue after at 0x68 as it was supposed to.
Another issue encountered was the queue arquitecture, as i was creating the queue after initializing the tasks, and the queue handle had to be send as a parameter to the tasks to be used.
