
# Entry Point for Serial Output
print("version32")

# Library and Package Imports
import time
import board
import busio
from digitalio import DigitalInOut, Direction, Pull
import analogio
import adafruit_mprls

# Startup LED Initialization
LED1 = DigitalInOut(board.D10)
LED1.direction = Direction.OUTPUT

# Pulse to 4 Bit Counter & Display
Pulse = DigitalInOut(board.D7)
Pulse.direction = Direction.OUTPUT

# Breadboard Pushbutton Stop
PB1 = analogio.AnalogIn(board.A2)
light_sensor = analogio.AnalogIn(board.A5)

# I2C Bus Initialization, Pressure Sensor Setup
i2c = busio.I2C(board.SCL, board.SDA)
mpr = adafruit_mprls.MPRLS(i2c, psi_min=0, psi_max=25)

# Global Variables
dt = 0.1
latched = False
latch_timer = 0
count = 0

# Data Variables
cars = []

def getTime(local_time):

    t = local_time
    timestamp = timestamp = "%d/%d/%d %d:%d:%d" % (t.tm_mon,t.tm_mday,t.tm_year,t.tm_hour,t.tm_min,t.tm_sec)

    return timestamp


# Startup Light Sequence
for i in range(3):
    LED1.value = True
    time.sleep(0.2)
    LED1.value = False
    time.sleep(0.2)

# Collect Data Until Stopped
while True:

    # Pushbutton Stop
    if PB1.value < 2000:
        break

    # Pressure Sensor Readout
    print("Pressure Sensor")
    print((mpr.pressure,))

    # Light Sensor Readout
    print("Light Sensor:")
    #print((light_sensor.value,))

    # Pressure Sensor Counting and Latch
    if (not latched):
        if mpr.pressure > 1005.00:

            count+= 1
            Pulse.value = 1
            Pulse.value = 0
            latched = True
            cars.append((count, getTime(time.localtime())))

    if latched:
        latch_timer += 1

    if (latch_timer*dt) > 2:
        latched = False
        latch_timer = 0

    time.sleep(dt)

print(cars)















