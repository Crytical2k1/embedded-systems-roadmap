import serial
from PyQt5.QtCore import QThread, pyqtSignal
from orientation_angles import get_rotation_angles


class SerialReader(QThread):
    orientation_updated = pyqtSignal(float, float, float)

    #set and start communication with serial
    def __init__(self, port='COM3', baudrate=9600):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.running = True

    def run(self):
        ser = serial.Serial(self.port, self.baudrate, timeout=1)

        while self.running:
            #read line from serial
            values = ser.readline().decode().strip()
            if not values:
                continue

            try:
                #ax, ay, az = map(float, values.split(','))
                #roll, pitch, yaw = get_rotation_angles(ax, ay, az)

                #extract values from serial line
                roll, pitch, yaw = map(float, values.split(','))
                #update values for 3d model
                self.orientation_updated.emit(-roll, pitch, yaw)

            except ValueError:
                continue

        ser.close()

    def stop(self):
        self.running = False
