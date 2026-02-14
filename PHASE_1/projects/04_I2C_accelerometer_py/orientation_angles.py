import math

def get_rotation_angles(ax, ay, az):
    """
    ax, ay, az: accelerometer values (gravity vector)
    returns roll, pitch, yaw in degress
    """
    ax, ay, az = align_axes(ax, ay, az)
    # Roll (rotation around x axis)
    roll = math.atan2(ay, az)

    #Pitch (rotation around Y axis)
    pitch = math.atan2(-ax, math.sqrt(ay*ay + az*az))

    #Yaw cannot be calculated just from the accelerometer values
    yaw = 0.0

    return (
        math.degrees(roll),
        math.degrees(pitch),
        yaw
    )

def align_axes(sx, sy, sz):
    """
    Convert sensor axes to OpenGL axes
    """
    ax = -sy
    ay = sx
    az = sz

    return ax, ay, az