Teensy Pinout: 

These are the codes running on the Teensy that is located next to the Extruder-unit on the Z- axis:

## Code_Teensy_Extruder.ino
- Main code running on the teensy, imports funcitons from the files below
- runs the main loop

## Gameover.ino
- safety watchdog 

## I2C_Master.ino
- ? 

## PID_TemperatureControl
- controls the hotend temperature via PID
- controls the Hotend relays by calling Helper function from Relays.ino 

## Motoren.ino
- ?

## Probe.ino
- BedLeveling Probe 

## PT100_MAX31865.ino
- Interfaces with MAX31865 boards and thermocouples for temperature reading

## Relays.ino
- Helper funcitons of relays for heaters, cooling, and light 

## RGB_Led.ino
- RGB-LED as a visual status indication located on the PCB

## RS485.ino
- Communication with the teensy located in the switch cabinet

## Tastertureingabe.ino
- ?

## TempWasser.ino
- control of water cooling temps 

## TM1637.ino
- 7 Segment displays located at the hotend 
    - these displays show hotend zone temps , water temps and other info 