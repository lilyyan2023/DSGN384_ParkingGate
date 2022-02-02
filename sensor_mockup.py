# Write your code here :-)
# Write your code here :-)

print('version 132')

import time
import board
from digitalio import DigitalInOut, Direction, Pull
import analogio

sensor = analogio.AnalogIn(board.A5)
while(1):

    print(sensor.value)
    time.sleep(5)

'''
A = DigitalInOut(board.D10)
A.direction = Direction.OUTPUT

# Startup Light
A.value = True
time.sleep(0.1)
A.value = False

B = DigitalInOut(board.D11)
B.direction = Direction.OUTPUT

'''





