# HugeFancy Teensy-Control 

Code that runs on the other microcontrollers and communicates with the Marlin-board.## HugeFancy Teensy-Control 

### Code_Teensy01_SwitchCabinet

Code running on the Teensy microcontroller located inside the switch cabinet. 
- Communicates with the Marlin-board via I2C
- Communicates with Teensy02 via RS485
- Communicates with the Arduino remote control via NRF24

### Code_Teensy02_Extruder

Code running on the Teensy microcontroller located on the Z-axis.
- Communicates with Teensy01 via RS485
- Handles temperature-control of the hotend
- Handles cooling-fan-control
- Controls color-mixing stepper-motors
- Controls Water-cooling circuit

### Code_Arduino_Joystick
Code running on the Arduino-Nano remote control we used during testing.