#include <Arduino.h>
#include <math.h>  
#include <Chrono.h>  // Include the Chrono library // https://github.com/SofaPirate/Chrono

unsigned long currentMillis, startMillis = 0;
int analog_resolution = 10; // sets resolution of analog writing as exponent of 2 (2^10=1024)


// Extruder temperatures
int TargetTempExtruderMarlin = 0; // max 9 Bit = 511° // Target Values from Marlin and Real Values from Hotend
int CombinedRealTempExtruder = 0; // max 9 Bit = 511°C
byte PwmValuePartCoolingFanMarlin = 0;

// Water cooling temperatures
int TempWatercooling_In = 0; // max 9 Bit = 511°C
int TempWatercooling_Out = 0; // max 9 Bit = 511°C

byte ExtruderCoolingStatusMarlin = 0; // 0 = off, 1 = on // muss 8 Bit, statt boolean sein

float percentageReservoir = 0;  // TODO: ask Chris/find out if this is actually in use or if the name is misleading

void setup()
{
  delay(200);
  Serial.begin(9600);
  delay(100);

  CoolingPWM_setup();
  CoolingPWM();

  analogReadResolution(analog_resolution);

  RS485_setup();
  TM1637_setup();
  Motoren_setup();
  PT100_MAX31865_setup();
  Relays_setup();
  RGB_setup();
  PID_setup();

  startMillis = millis(); //start timer for periodic executions
  Serial.println("Setup completed");
}

void loop()
{
  currentMillis = millis(); // used for the periodic execution of different functions

  RS485_Extruder_CheckIfUpdateAvalible();
  PT100_MAX31865_loop();
  TM1637_update();
  TempWater_loop();
  PID_loop();
  CombineRealTemps();
  Stepper_loopWatchdogDisableSteppers();
  Stepper_loopMetronomeColor();


  //watchdog_gameover();

  SerialTastatur_CheckKeys();

  if (currentMillis - startMillis > 1000)
    {
    //Serial.print("RealTemperatureZone_1 = ");Serial.println(TargetTemperatureZone_1);
    //Serial.print("RealTemperatureZone_2 = ");Serial.println(TargetTemperatureZone_2);
    //PrintCombTemps();
    Serial.println("1000ms have passed");
    startMillis = currentMillis;
    }
}
