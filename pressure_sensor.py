

print('version 132')

import time
import board
from digitalio import DigitalInOut, Direction, Pull
import analogio
import busio
import adafruit_mprls

# Startup LED
LED1 = DigitalInOut(board.D10)
LED1.direction = Direction.OUTPUT

# Initialize i2c pins and MPRLS library
i2c = busio.I2C(board.SCL, board.SDA)
mpr = adafruit_mprls.MPRLS(i2c, psi_min=0, psi_max=25)

# Startup Light
for i in range(3):
    LED1.value = True
    time.sleep(0.2)
    LED1.value = False
    time.sleep(0.2)

count = 0

while(True):

    print("Pressure (hPa):", mpr.pressure)


