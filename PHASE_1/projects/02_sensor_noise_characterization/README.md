# ADC noise signal characterization

for this project the signal was obtained as a floating analog pin, therefore the signal depends on thermal noise, quatization, coupling from ADC clock, MCU core, power supply...
The signal was measured with 3 different sample periods 5ms, 20ms and 100ms to compare the behaviour.

## Hardware

Elegoo/Arduino Uno R3 (ATMega328P)

## Code Set up

- Hardware set up

1. Timer
   First thing is create a timer that triggers an interrupt and keep track of a counter, this will allow us to control the time and sample period.
2. GPIO
   As a visual representation of the sampling period i set up the onboard LED that is toggled at sample rate.
3. Serial
   Next setting up the serial communication to allow transmition of data setting the baud rate and sending basic data as a debugging tool
4. ADC
   Setting the AD converter, using a floating analog pin (to code view is not relevant, just in terms of hardware). With the values obtained are sent thourgh serial communication as raw data.
5. Serial (revisited)
   To finish up the hardware set up, to modify at runtime the sample period had to set up the serial to also allow recieve information without breaking the usage of the transmision logic
6. Main loop
   Inside the main loop just 2 actions are taking place, handle recieve data thought serial and handle sampling data. This 2 functions are developed in an app.c file that is in charge of check if data is available to read from serial, if so modifies the sampling period if the command format is right and resets the sampling time to avoid extra samples or missing data. On the other side, handling sampling data converts analog to digital and sends it through serial at sampling rate.

- Signal processing set up

1. Data storage
   In a python file the logic to read everything recieved on the serial monitor is stored in a .csv file for later study and processing.
2. Signal study
   In a second file, the data is plotted, as the raw data and a histogram to check the distribution, the noise amplitud is calculated as well as the mean and standart distribution, and a FFT is calculated plotting the frequencies, finishing with a plot of the autocorrelation of the data.
   All this information grants useful insight of the behaviour of the data across the different sample rates, comming to the conclusion that the noise observed is not random but deterministic and can observe clearly aliasing present in the readings, achieving the goal of this project.

## Analysis of the signal

After ploting the signal with the time in the X axis and the ADC value in the Y axis, i could clearly observe that for the first 2s the system was stil settling into the wave distribution seen later in time, so for the study of the signal i removed this first portion of the data.

The 3 signals dont look like random data scattered across time, but more of a periodic variation in time, therefore we could think that the data is not purely random noise but deterministic interference, therefore the floating analog pin readings (in first sight should have been random noise) but ended up being correlated and structured.

I could calculate the amplitud of the noise for each sampling period giving a value between 12-14.

After histogram representation it has a clear non-Gaussian distribution, presenting several peaks being likely the presence of structured interference.

Calculating the mean value of the ADC signal is clearly not centered and it changes slightly through sample periods, this could be due to different factor (taking into consideration that the experiments where run one after the other one, not simultaneously) including floating input, ADC reference value instability and coupling with the digitial circuitry.

After the FFT analysis we can distinguish to scenarios, fast sampling (5ms) has a strong peak at high frecuency while medium and slow sampling (20ms - 100ms) shows a peak at low frecuencies. Demonstrating the presence of aliasing.
As conclusion, the noise frequency depends strongly on sampling rate, indicating aliasing of deterministic interference.

In the last plotting the autocorrelation is strong and structured as all the points draw a line not a random cloud of points. This shows that consecutive samples are dependent and noise is highly correlated in time. Demonstrating the absence of random/white noise.

As a conclusion we can agree that the measured ADC noise (through a floating analog pin) is dominated by deterministic, low-frequency interference (proper of electrical circuitry), and not random noise. Sampling rate strongly affects the behaviour of the reading due to aliasing.

## Problems encountered

The issue that appear was to store the data sent by the arduino for later study as 2 different systems had to be connected for this feature, the arduino code running and the python file running.
