import sys
import math
from PyQt5.QtWidgets import QMainWindow, QApplication, QVBoxLayout, QWidget, QPushButton
from PyQt5.QtOpenGL import QGLWidget
from OpenGL.GL import *
from orientation_log import SerialReader


class GLWidget(QGLWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.roll = 0.0
        self.pitch = 0.0
        self.yaw = 0.0
        self.startTimer(16)  # ~60 FPS

    def set_orientation(self, roll, pitch, yaw):
        self.roll = roll
        self.pitch = pitch
        self.yaw = yaw

    def initializeGL(self):
        glClearColor(0.0, 0.0, 0.0, 1.0)
        glEnable(GL_DEPTH_TEST)

    def resizeGL(self, width, height):
        glViewport(0, 0, width, height)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()

        aspect = width / height if height else 1
        fov = 45.0
        near = 0.1
        far = 100.0

        top = near * math.tan(math.radians(fov / 2))
        bottom = -top
        left = bottom * aspect
        right = top * aspect

        glFrustum(left, right, bottom, top, near, far)
        glMatrixMode(GL_MODELVIEW)

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glLoadIdentity()

        glTranslatef(0.0, 0.0, -5.0)
        #rotations base on the perspective and view of my device and the model
        glRotatef(self.roll, 0, 0, 1)
        glRotatef(self.pitch, 1, 0, 0)
        glRotatef(self.yaw, 0, 1, 0)

        glBegin(GL_QUADS)

        # Top
        glColor3f(1, 0, 0)
        glVertex3f(1, 0.2, -1)
        glVertex3f(-1, 0.2, -1)
        glVertex3f(-1, 0.2, 1)
        glVertex3f(1, 0.2, 1)

        # Bottom
        glColor3f(0, 1, 0)
        glVertex3f(1, -0.2, 1)
        glVertex3f(-1, -0.2, 1)
        glVertex3f(-1, -0.2, -1)
        glVertex3f(1, -0.2, -1)

        # Front
        glColor3f(0, 0, 1)
        glVertex3f(1, 0.2, 1)
        glVertex3f(-1, 0.2, 1)
        glVertex3f(-1, -0.2, 1)
        glVertex3f(1, -0.2, 1)

        # Back
        glColor3f(1, 1, 0)
        glVertex3f(1, -0.2, -1)
        glVertex3f(-1, -0.2, -1)
        glVertex3f(-1, 0.2, -1)
        glVertex3f(1, 0.2, -1)

        # Left
        glColor3f(0, 1, 1)
        glVertex3f(-1, 0.2, 1)
        glVertex3f(-1, 0.2, -1)
        glVertex3f(-1, -0.2, -1)
        glVertex3f(-1, -0.2, 1)

        # Right
        glColor3f(1, 0, 1)
        glVertex3f(1, 0.2, -1)
        glVertex3f(1, 0.2, 1)
        glVertex3f(1, -0.2, 1)
        glVertex3f(1, -0.2, -1)

        # Z-axis
        glColor3f(1, 1, 1)
        glVertex3f(-0.01, 0, 2)
        glVertex3f(0.01, 0, 2)
        glVertex3f(-0.01, 0, 0)
        glVertex3f(0.01, 0, 0)
        # X-axis
        glColor3f(1, 1, 1)
        glVertex3f(2, -0.01, 0)
        glVertex3f(2, 0.01, 0)
        glVertex3f(0, -0.01, 0)
        glVertex3f(0, 0.01, 0)
        # Y-axis
        glColor3f(1, 1, 1)
        glVertex3f(-0.01, 2, 0)
        glVertex3f(0.01, 2, 0)
        glVertex3f(-0.01, 0, 0)
        glVertex3f(0.01, 0, 0)


        glEnd()

    def timerEvent(self, event):
        self.update()


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("3D Device Orientation Modeling")
        self.setGeometry(100, 100, 800, 600)

        layout = QVBoxLayout()
        container = QWidget()
        container.setLayout(layout)
        self.setCentralWidget(container)

        self.glWidget = GLWidget()
        layout.addWidget(self.glWidget)

        self.serial_thread = SerialReader()
        self.serial_thread.orientation_updated.connect(
            self.glWidget.set_orientation
        )
        self.serial_thread.start()

    def closeEvent(self, event):
        self.serial_thread.stop()
        self.serial_thread.wait()
        event.accept()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
