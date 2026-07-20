import argparse
import math
import threading
import time
from collections import deque

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
import serial
from mpl_toolkits.mplot3d.art3d import Poly3DCollection

# ---------------------------------------------------------------------------
# Shared state, updated by the serial-reading thread, read by the plot loop
# ---------------------------------------------------------------------------

class Telemetry:
    def __init__(self):
        self.lock = threading.Lock()
        self.heading_deg = 0.0
        self.light_error_deg = 0.0
        self.target_deg = 0.0
        self.last_update = time.time()
        self.connected = False

    def update(self, heading, light_error, target):
        with self.lock:
            self.heading_deg = heading
            self.light_error_deg = light_error
            self.target_deg = target
            self.last_update = time.time()

    def snapshot(self):
        with self.lock:
            return self.heading_deg, self.light_error_deg, self.target_deg, self.last_update


def serial_reader(port, baud, telemetry, stop_event):
    """Background thread: continuously read and parse TLM lines."""
    while not stop_event.is_set():
        try:
            with serial.Serial(port, baud, timeout=1) as ser:
                telemetry.connected = True
                print(f"[serial] connected to {port} @ {baud}")
                while not stop_event.is_set():
                    raw = ser.readline().decode(errors="ignore").strip()
                    if not raw.startswith("TLM,"):
                        continue
                    parts = raw.split(",")
                    if len(parts) != 4:
                        continue
                    try:
                        heading = float(parts[1])
                        light_error = float(parts[2])
                        target = float(parts[3])
                    except ValueError:
                        continue
                    telemetry.update(heading, light_error, target)
        except serial.SerialException as e:
            telemetry.connected = False
            print(f"[serial] error: {e} — retrying in 2s")
            time.sleep(2)


# ---------------------------------------------------------------------------
# 3D geometry helpers
# ---------------------------------------------------------------------------

def rotated_box(center, size, heading_deg):
    """Return the 6 faces (as Poly3DCollection-ready vertex lists) of a box
    rotated about the Z axis by heading_deg, representing the device body."""
    l, w, h = size
    # box corners centered at origin
    corners = np.array([
        [-l/2, -w/2, -h/2], [ l/2, -w/2, -h/2], [ l/2,  w/2, -h/2], [-l/2,  w/2, -h/2],
        [-l/2, -w/2,  h/2], [ l/2, -w/2,  h/2], [ l/2,  w/2,  h/2], [-l/2,  w/2,  h/2],
    ])
    theta = math.radians(heading_deg)
    rot = np.array([
        [math.cos(theta), -math.sin(theta), 0],
        [math.sin(theta),  math.cos(theta), 0],
        [0, 0, 1],
    ])
    corners = corners @ rot.T + np.array(center)

    faces = [
        [corners[0], corners[1], corners[2], corners[3]],  # bottom
        [corners[4], corners[5], corners[6], corners[7]],  # top
        [corners[0], corners[1], corners[5], corners[4]],  # side
        [corners[2], corners[3], corners[7], corners[6]],  # side
        [corners[1], corners[2], corners[6], corners[5]],  # side
        [corners[3], corners[0], corners[4], corners[7]],  # side
    ]
    return faces


def heading_arrow(center, heading_deg, length):
    theta = math.radians(heading_deg)
    dx, dy = length * math.cos(theta), length * math.sin(theta)
    return center, (center[0] + dx, center[1] + dy, center[2])


def light_position(heading_deg, light_error_deg, radius, height):
    """Estimated light-source position: relative bearing (light_error) is
    measured off the device's current heading."""
    theta = math.radians(heading_deg + light_error_deg)
    x = radius * math.cos(theta)
    y = radius * math.sin(theta)
    return x, y, height


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description="3D live viewer for ESP32 light tracker")
    parser.add_argument("--port", default='COM3', help="Serial port, e.g. /dev/ttyUSB0 or COM5")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--radius", type=float, default=3.0, help="Distance to draw the light source at")
    args = parser.parse_args()

    telemetry = Telemetry()
    stop_event = threading.Event()
    reader_thread = threading.Thread(
        target=serial_reader, args=(args.port, args.baud, telemetry, stop_event), daemon=True
    )
    reader_thread.start()

    fig = plt.figure(figsize=(8, 8))
    ax = fig.add_subplot(111, projection="3d")

    box_size = (0.6, 0.4, 0.25)
    device_center = (0, 0, 0)
    light_height = 0.3

    history = deque(maxlen=200)  # trail of recent light positions

    def draw(frame):
        ax.cla()
        heading, light_error, target, last_update = telemetry.snapshot()
        stale = (time.time() - last_update) > 2.0

        ax.set_xlim(-args.radius - 1, args.radius + 1)
        ax.set_ylim(-args.radius - 1, args.radius + 1)
        ax.set_zlim(-1, args.radius + 1)
        ax.set_box_aspect((1, 1, 0.6))
        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.set_zlabel("Z")

        # device body
        faces = rotated_box(device_center, box_size, heading)
        box = Poly3DCollection(faces, facecolor="steelblue", edgecolor="black", alpha=0.9)
        ax.add_collection3d(box)

        # heading arrow (where the device is currently facing)
        start, end = heading_arrow(device_center, heading, args.radius * 0.4)
        ax.quiver(*start, end[0]-start[0], end[1]-start[1], end[2]-start[2],
                   color="dodgerblue", linewidth=2, arrow_length_ratio=0.15, label="Heading")

        # target arrow (where the angle controller is steering toward)
        tstart, tend = heading_arrow(device_center, target, args.radius * 0.4)
        ax.quiver(*tstart, tend[0]-tstart[0], tend[1]-tstart[1], tend[2]-tstart[2],
                   color="orange", linewidth=1.5, arrow_length_ratio=0.15, linestyle="dashed", label="Target")

        # light source
        lx, ly, lz = light_position(heading, light_error, args.radius, light_height)
        history.append((lx, ly, lz))
        ax.scatter([lx], [ly], [lz], color="gold", edgecolor="darkorange", s=250, marker="o", label="Light source")
        ax.plot([device_center[0], lx], [device_center[1], ly], [device_center[2], lz],
                color="gray", linestyle="--", linewidth=1)

        # faint trail of where the light has been estimated
        if len(history) > 1:
            hx, hy, hz = zip(*history)
            ax.plot(hx, hy, hz, color="gold", alpha=0.25, linewidth=1)

        status = "STALE (no data)" if stale else ("LIVE" if telemetry.connected else "connecting…")
        ax.text2D(
            0.02, 0.95,
            f"[{status}]  heading={heading:6.1f}°  light_error={light_error:6.1f}°  target={target:6.1f}°",
            transform=ax.transAxes, fontsize=10, family="monospace",
        )
        ax.legend(loc="upper right")
        ax.set_title("Light-Tracking Robot — Live 3D View")

    ani = FuncAnimation(fig, draw, interval=100)

    try:
        plt.show()
    finally:
        stop_event.set()


if __name__ == "__main__":
    main()