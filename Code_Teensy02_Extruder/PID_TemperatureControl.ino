/*
 * Code that is responsible for controling the hotend heat-bands with PID
 * 
 * PID Example: https://github.com/br3ttb/Arduino-PID-Library/blob/master/examples/PID_RelayOutput/PID_RelayOutput.ino
 * relevant Forum post: https://forum.arduino.cc/index.php?topic=142526.0 
 */

/********************************************************
 * PID RelayOutput for Temperature control
 * the pid library is designed to Output an analog value,
 * but the relay can only be On/Off.
 *
 *   to connect them together we use "time proportioning
 * control"  it's essentially a really slow version of PWM.
 * first we decide on a window size (5000mS say.) we then
 * set the pid to adjust its output between 0 and that window
 * size.  lastly, we add some logic that translates the PID
 * output into "Relay On Time" with the remainder of the
 * window being "Relay Off Time"
 ********************************************************/

#include <PID_v1.h>

//see Relays.ino for pinout 
const int Zone1_TargetOffset = 5 ;  // Heating Zone 1 will be different from Zone 2
int TargetTemperatureZone_1 = 0; // max 9 Bit = 511°C
int TargetTemperatureZone_2 = 0; // max 9 Bit = 511°C

// ->
int RealTemperatureZone_1 = 0; // max 9 Bit = 511°C
int RealTemperatureZone_2 = 0; // max 9 Bit = 511°C
double AveragedRealTempZone_1 = 0; //average temperature over the last 5 measurements
double AveragedRealTempZone_2 = 0;
double HeatPowerZone_1 = 0;
double HeatPowerZone_2 = 0;

/// Hotend Bools
// these are status bools  that do not affect the actual prcess but simply indicate a status.
// Changing them here will not activate the relays
int HeatFanZone_1 = 0;
int HeatFanZone_2 = 0;
int HeaterZone_1 = 0;
int HeaterZone_2 = 0;

//Define Variables we'll be connecting to
double Setpoint_Zone1, Input_Zone1, Output_Zone1; //Heating Zone 1
double Setpoint_Zone2, Input_Zone2, Output_Zone2; //Heating Zone 2

//Specify the links and initial tuning parameters 
//TODO: find out if P_ON_M is needed (proportional on measurement) 
//    http://brettbeauregard.com/blog/2017/06/introducing-proportional-on-measurement/

//Heating Zone_1
double Kp_Zone1=120, Ki_Zone1=0.3274, Kd_Zone1=150;
PID myPID_Zone1(&Input_Zone1, &Output_Zone1, &Setpoint_Zone1, Kp_Zone1, Ki_Zone1, Kd_Zone1, 1, DIRECT);

//Heating Zone_2
double Kp_Zone2=120, Ki_Zone2=0.3274, Kd_Zone2=150;
PID myPID_Zone2(&Input_Zone2, &Output_Zone2, &Setpoint_Zone2, Kp_Zone2, Ki_Zone1, Kd_Zone2, 1, DIRECT);

int WindowSize = 4000;  // the "time proportioning control" window
unsigned long windowStartTime;
double averageZ1,averageZ2;
double OutputBufferZ1, OutputBufferZ2;

void PID_setup()
{
  windowStartTime = millis();

  //initialize the variables we're linked to
  Setpoint_Zone1 = 0;
  Setpoint_Zone2 = 0;

  //tell the PID to range between 0 and the full window size
  myPID_Zone1.SetOutputLimits(0, WindowSize);
  myPID_Zone2.SetOutputLimits(0, WindowSize);


  //turn the PID on
  myPID_Zone1.SetMode(AUTOMATIC);
  myPID_Zone2.SetMode(AUTOMATIC);
}

void PID_loop()
{ 

  //Set the current temperature targets
  Setpoint_Zone1 = (double)TargetTemperatureZone_1;
  Setpoint_Zone2 = (double)TargetTemperatureZone_2;

  //Get the current temperature 
  //Input_Zone1 = RealTemperatureZone_1;
  //Input_Zone2 = RealTemperatureZone_2;
  Input_Zone1 = AveragedRealTempZone_1;
  Input_Zone2 = AveragedRealTempZone_2;

  //Compute PID from Input Values 
  myPID_Zone1.Compute();
  myPID_Zone2.Compute();

  if (Output_Zone1/WindowSize < 0.5 && Input_Zone1 > Setpoint_Zone1 *1.01)
  {
    RelayCoolerZone_1_SetStatus(true) ;
  }
  else
  {
   RelayCoolerZone_1_SetStatus(false) ;
  }
  
  if (Output_Zone2/WindowSize < 0.5 && Input_Zone2 > Setpoint_Zone2 *1.01)
  {
    RelayCoolerZone_2_SetStatus(true) ;
  }
  else
  {
   RelayCoolerZone_2_SetStatus(false) ;
  }

  /************************************************
   * turn the output pin on/off based on pid output
   ************************************************/
  if ( (abs(millis() - windowStartTime) > WindowSize) )
  { //time to shift the Relay Window
    windowStartTime = millis();
  }

  OutputBufferZ1 = (OutputBufferZ1 + Output_Zone1) / 2;
  HeatPowerZone_1 = OutputBufferZ1/WindowSize *25;
  if (OutputBufferZ1 > millis() - windowStartTime) 
  { 
    RelayHeaterZone_1_SetStatus(true);
    //RelayCoolerZone_1_SetStatus(false) ;
    HeatFanZone_1 = 0;
    HeaterZone_1 = 15;
    //Serial.print(15);
    //Serial.print(",");

  }
  else 
  {
    RelayHeaterZone_1_SetStatus(false);
    //RelayCoolerZone_1_SetStatus(true) ;
    HeatFanZone_1 = 18;
    HeaterZone_1 = 0;
    //Serial.print(0);
    //Serial.print(",");   
  }
  OutputBufferZ2 = (OutputBufferZ2 + Output_Zone2) / 2;
  HeatPowerZone_2 = OutputBufferZ2/WindowSize *25;
  if (OutputBufferZ2 > millis() - windowStartTime) 
  { 
    RelayHeaterZone_2_SetStatus(true);
    //RelayCoolerZone_2_SetStatus(false) ;
    HeatFanZone_2 = 0;
    HeaterZone_2 = 10;
    //Serial.print(10);
    //Serial.print(",");
  }
  else 
  {
    RelayHeaterZone_2_SetStatus(false);
    //RelayCoolerZone_2_SetStatus(true) ;
    HeatFanZone_2 = 7;
    HeaterZone_2 = 0;  
    //Serial.print(0);
    //Serial.print(",");       
  }
}
