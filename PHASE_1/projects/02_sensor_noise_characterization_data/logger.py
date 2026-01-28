import serial

PORT = "COM3"
BAUD = 9600

ser = serial.Serial(PORT, BAUD, timeout=1)

with open("data.csv", "w") as f:
    f.write("time_ms,adc\n") #header

    print("Logging... Ctrl+C to stop")
    try:
        while True:
            line = ser.readline().decode().strip()
            if line:
                f.write(line + "\n")
                print(line)
    except KeyboardInterrupt:
        print("Stopped")
ser.close()