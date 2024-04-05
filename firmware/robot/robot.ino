// A very simple Serial-over-BT app that reads input from the host and CAPITALIZES it.
// Released to the public domain by Earle F. Philhower, III, in February 2023

// Under Linux to connect to the PicoW
// 1. Pair to the "PicoW Serial XX:XX..." device using your favorite GUI, entering a PIN of "0000"
// 2. Execute "sudo rfcomm bind 0 00:00:00:00:00:00" to make a `/dev/rfcomm0" device, replacing the "00:00.." with the MAC as listed in the device name
// 3. Run "minicom -D /dev/rfcomm0" and type away
// 4. To remove the virtual serial port, execute "sudo rfcomm release rfcomm0"

// Under Windows to connect to the PicoW
// 1. Pair to the "PicoW Serial XX:XX..." device using the copntrol panel, ignoring any PIN it says to check for
// 2. At this point you will have a new COM: port.  You may need to use the Device Manager to find it's number.
// 3. Open up COMX: in your favorite terminal application and type away

// Under Mac to connect to the PicoW
// 1. Open System Preferences and go in the bluetooth section. You should find a bluetooth device called
//    PicoW Serial XX:XX:... Click Connect button.
// 2. A /dev/tty.PicoWSerialXXXX becomes available.
// 3. Connect to this device with your favorite terminal application.

#include "globals.h"
#include "telemetry.h"

void setup() {
  Serial.begin();
  SerialBT.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(20, OUTPUT);
  pinMode(21, OUTPUT);
  init_telemetry();
}
int state = HIGH;
void loop() {
  while (Serial) {
    
    if (check_for_telemetry()){
      state = ~state;
      digitalWrite(LED_BUILTIN, state); 
    }
    else
    {
      process_telemetry();
    }
  }
}
