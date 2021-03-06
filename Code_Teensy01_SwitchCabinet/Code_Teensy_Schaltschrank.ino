#include <Arduino.h>

unsigned long currentMillis = 0;
unsigned long RS485_sent_LastUpdatePreviousMillis = 0;
#include <Metro.h> // Include the Metro library // https://www.pjrc.com/teensy/td_libs_Metro.html

// Global variables, TargetTemp from Marlin and RealTemp from Hotend

int TargetTempExtruderMarlin = 0; // max 9 Bit - 1 = 510°C
int CombinedRealTempExtruder = 0; // max 9 Bit - 1 = 510°C

int TargetTemperatureZone_1 = 0; // max 8 Bit = 255°C
int TargetTemperatureZone_2 = 0; // max 8 Bit = 255°C
int RealTemperatureZone_1 = 22; // max 8 Bit = 255°C // TEST ### ToDo
int RealTemperatureZone_2 = 33; // max 8 Bit = 255°C


byte PwmValuePartCoolingFanMarlin = 0;  // not yet connected with Marlin //TODO Robin

void setup()
{
  Serial.begin(9600); 
  delay(100);

  RS485_setup();
  setup_Funk();
  RGB_setup();
  Relays_setup();
  I2C_Marlin_setup();

  Serial.println("Setup Done, starting loop");
}

void loop()
{
  SerialTastatur_CheckKeys();
  loop_RS485_Switchcabinet_Send_Statusupdate();
  RS485_Switchcabinet_CheckIfUpdateAvalible();
  loop_FunkCheck();
  //watchdog_gameover();
}
