# Library and Package Imports
import time
import board
import busio
from digitalio import DigitalInOut, Direction, Pull
import analogio
import adafruit_mprls
import pwmio
from adafruit_motor import servo

# Entry Point for Serial Output
print("version32")


### Pin Initialization ################################################################
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

# Create a PWMOut object on Pin A2.
pwm = pwmio.PWMOut(board.A1, duty_cycle=2 ** 15, frequency=50)
# Create a servo object, my_servo.
my_servo = servo.Servo(pwm)


### Global Variables ##################################################################
dt = 0.1  # timestep
latched = False  # pressure sensor latch
daytime = False  # light sensor latch
latch_timer = 0
count = 0
time_0 = time.monotonic()  # initial time


### Data Variables ####################################################################
car_data = []
time_data = []
pressure_data = []
light_data = []


### Helper Functions ##################################################################
def getTime(local_time):

    t = local_time
    timestamp = timestamp = "%d/%d/%d %d:%d:%d" % (t.tm_mon, t.tm_mday, t.tm_year, t.tm_hour, t.tm_min, t.tm_sec)

    return timestamp


### Startup Light Sequence ############################################################
for i in range(3):
    LED1.value = True
    time.sleep(0.2)
    LED1.value = False
    time.sleep(0.2)

for angle in range(50, 180, 2):
    my_servo.angle = angle
    time.sleep(0.02)

for angle in range(180, 50, -2):
    my_servo.angle = angle
    time.sleep(0.02)


### Sensor Loop #######################################################################

while True:

    ### Pushbutton Stop ###
    if PB1.value < 2000:
        break

    ### Sensor Data Collection ###
    time_data.append(time.monotonic() - time_0)  # time
    pressure_data.append(mpr.pressure)         # pressure
    light_data.append(light_sensor.value)      # light

    ### Sensor Readouts ###
    #print("Pressure Sensor")
    #print((mpr.pressure,))
    #print("Light Sensor:")
    #print((light_sensor.value,))

    # Open the Gate at Dawn
    if (not daytime) and light_sensor.value > 500:

        for angle in range(50, 180, 2):
            my_servo.angle = angle
            time.sleep(0.02)

        daytime = True

    # Close the Gate at Dusk
    if daytime and light_sensor.value < 100:
        for angle in range(180, 50, -2):
            my_servo.angle = angle
            time.sleep(0.02)

        daytime = False

    # Pressure Sensor Counting and Latch
    if (not latched):
        if mpr.pressure > 1005.00:

            count+= 1
            Pulse.value = 1
            Pulse.value = 0
            latched = True
            car_data.append((count, getTime(time.localtime())))

    if latched:
        latch_timer += 1

    if (latch_timer*dt) > 2:
        latched = False
        latch_timer = 0

    # Timestep
    time.sleep(dt)

print(time_data)
print(pressure_data)
print(light_data)
print(car_data)















