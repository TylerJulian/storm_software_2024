import pygame
# Initialize the joysticks.
import time
import serial

from telemetry import *


pygame.init()
pygame.joystick.init()

ser = serial.Serial('COM9')  # open serial port


# Get count of joysticks.
joystick_count = pygame.joystick.get_count()

command_packet = CommandStruct()

if joystick_count > 0:
    joy = pygame.joystick.Joystick(0)
    joy.init()

    # Get the name from the OS for the controller/joystick.
    name = joy.get_name()
    print("Joystick name: {}".format(name))

    # Usually axis run in pairs, up/down for one, and left/right for
    # the other.
    axes = joy.get_numaxes()
    print("Number of axes: {}".format(axes))
    buttons = joy.get_numbuttons()
    hats = joy.get_numhats()

    # For each joystick:
    while True:
        time.sleep(.05)
        pygame.event.get()
        command_packet.command_id = CommandIDs.drive_command
        command_packet.data.trigger_control.left = joy.get_axis(4) + 1
        command_packet.data.trigger_control.right = joy.get_axis(5) + 1
        ser.write(command_packet.raw)
        print(command_packet.raw)

    ser.close()             # close port