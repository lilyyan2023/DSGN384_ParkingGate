
print('version 132')

import time
import board
from digitalio import DigitalInOut, Direction, Pull
import analogio

from adafruit_onewire.bus import OneWireBus
from adafruit_ds18x20 import DS18X20

# Startup LED
LED1 = DigitalInOut(board.D10)
LED1.direction = Direction.OUTPUT

# Pulse to 4 Bit Counter & Display
Pulse = DigitalInOut(board.D7)
Pulse.direction = Direction.OUTPUT

# Pushbutton stop
PB1 = analogio.AnalogIn(board.A2)
sensor = analogio.AnalogIn(board.A5)

# Initialize one-wire bus on board pin D5.
ow_bus = OneWireBus(board.D5)
ds18 = DS18X20(ow_bus, ow_bus.scan()[0])

# Startup Light
for i in range(3):
    LED1.value = True
    time.sleep(0.2)
    LED1.value = False
    time.sleep(0.2)

count = 0

while(PB1.value > 2000):

    print((sensor.value,))
    time.sleep(0.1)

    if sensor.value > 200:
        Pulse.value = True
        Pulse.value = False
        count += 1
        print("Car #", count, "\n")
        time.sleep(2)





while True:
    print((ds18.temperature,))
    time.sleep(1)






