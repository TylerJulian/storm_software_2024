from pygame import joystick
# Initialize the joysticks.
joystick.init()


# -------- Main Program Loop -----------
while not done:


    # Get count of joysticks.
    joystick_count = joystick.get_count()

    # For each joystick:

    if joystick > 0:
        joy = joystick.Joystick(0)
        joy.init()


        # Get the name from the OS for the controller/joystick.
        name = joy.get_name()
        print("Joystick name: {}".format(name))

        # Usually axis run in pairs, up/down for one, and left/right for
        # the other.
        axes = joysjoytick.get_numaxes()
        print("Number of axes: {}".format(axes))


        for i in range(axes):
            axis = joystick.get_axis(i)
            print(screen, "Axis {} value: {:>6.3f}".format(i, axis))

        buttons = joystick.get_numbuttons()
        print("Number of buttons: {}".format(buttons))

        for i in range(buttons):
            button = joystick.get_button(i)
            print("Button {:>2} value: {}".format(i, button))

        hats = joystick.get_numhats()
        print("Number of hats: {}".format(hats))

        # Hat position. All or nothing for direction, not a float like
        # get_axis(). Position is a tuple of int values (x, y).
        for i in range(hats):
            hat = joystick.get_hat(i)
            print("Hat {} value: {}".format(i, str(hat)))