// Schrittmotoren für den ColorMixing ansteuern

#include <TeensyStep.h>
// Git: https://github.com/luni64/TeensyStep DOCUMENTATION

// PINOUTMotorSpeed

// Motor Right = F1
const int M_R_EN = 33; // Enable Pin
Stepper M_R(34, 35); // Step, Dir

// Motor Left = F2
const int M_L_EN = 29; // Enable Pin //15
Stepper M_L(27, 28); // Step, Dir // 16,17

StepControl StepController; // Controller, Stepping Mode

/*
  // Motor Zusatz
  const int M_Zusatz_EN = 26; // Enable Pin
  const int M_Zusatz_Endstop = 25; // Endstop Pin
  Stepper M_Zusatz(30, 31); // Step, Dir
*/

/*
  // NANOTEC
  const int microstepping_nanotec = 64; // Stepper Nanotec 16-fach
  const int Nanotec_MaxSpeed = 800; // Stepper Nanotec z.B. 800
  const int Nanotec_Acceleration = 500; // Stepper Nanotec z.B. 500
  // Enable Funktion bei Nanotec invertiert.
*/


const int motorsteps = 200; // 200 Schrittmotor, 16 Servo
const int gear_ratio = 1; // >0 entspricht Untersetzung

const int M_LR_microstepping = 1; // TMC 2209 16 = Standard ohne Jumper
const int M_LR_MaxSpeed = 40;
const int M_LR_Acceleration = 50;
int MotorSpeed = M_LR_MaxSpeed; // Revolutions, speed steps/s,
int MotorAcceleration = M_LR_Acceleration; // acceleration mm/s^2


boolean ColorMixingMetronomeColor = false;
int ColorTimeSeconds_L = 0;
int ColorTimeSeconds_R = 0;
int ColorTimeSeconds_shift = 0;

const float Anzahl_ColShovel = 1.8;

// Metronome Color Intervals
Chrono Chrono_ColorTimeSeconds_L;
Chrono Chrono_ColorTimeSeconds_R;

Chrono Chrono_StepperWatchdog;


void Motoren_setup()
{
  pinMode (M_L_EN, OUTPUT);
  pinMode (M_R_EN, OUTPUT);
  // NANOTEC
  //pinMode (M_nanotec_EN, OUTPUT);
  delay(10);

  Schrittmotor_L_aktiv(false);
  Schrittmotor_R_aktiv(false);
  // NANOTEC
  // Schrittmotor_nanotec_aktiv(false);

  // Speed [stp/s] // Acceleration [stp/s^2]
  M_L.setMaxSpeed(M_LR_MaxSpeed);
  M_L.setAcceleration(M_LR_Acceleration);
  M_R.setMaxSpeed(M_LR_MaxSpeed);
  M_R.setAcceleration(M_LR_Acceleration);
  // NANOTEC
  // M_nanotec.setMaxSpeed(Nanotec_MaxSpeed);
  // M_nanotec.setAcceleration(Nanotec_Acceleration);

  //Chrono_ColorTimeSeconds_L.stop();
  //Chrono_ColorTimeSeconds_R.stop();
}

void Schrittmotor_L_aktiv(bool setStatus)
{
  if (setStatus == true)
  {
    digitalWrite(M_L_EN, LOW);
    delay(10);
  }
  else if (setStatus == false)
  {
    digitalWrite(M_L_EN, HIGH);
  }
}

void Schrittmotor_L(int Revolutions, int Speed, int Acceleration)
{
  Schrittmotor_L_aktiv(true);
  M_L.setMaxSpeed(Speed); // stp/s
  M_L.setAcceleration(Acceleration); // stp/s^2



  M_L.setTargetRel(Revolutions * gear_ratio * motorsteps * M_LR_microstepping); // 1600 = 1 REV with TMC2209
  StepController.moveAsync(M_L);

  M_L.setMaxSpeed(M_LR_MaxSpeed);         // stp/s
  M_L.setAcceleration(M_LR_Acceleration); // stp/s^2
  //Schrittmotor_L_aktiv(false);
}

void Schrittmotor_R_aktiv(bool value)
{
  if (value == true)
  {
    digitalWrite(M_R_EN, LOW);
    delay(10);
  }
  else if (value == false)
  {
    digitalWrite(M_R_EN, HIGH);
  }
}

void Schrittmotor_R(int Revolutions, int Speed, int Acceleration)
{
  Schrittmotor_R_aktiv(true);
  M_R.setMaxSpeed(Speed); // stp/s
  M_R.setAcceleration(Acceleration); // stp/s^2

  M_R.setTargetRel(Revolutions * motorsteps * gear_ratio * M_LR_microstepping); // 1600 = 1 REV with TMC2209
  StepController.moveAsync(M_R);

  M_R.setMaxSpeed(M_LR_MaxSpeed); // stp/s
  M_R.setAcceleration(M_LR_Acceleration); // stp/s^2
  //Schrittmotor_R_aktiv(false);
}


void ColorMixing_AddColor(int AddColShovel_R, int AddColShovel_L)
{
  Schrittmotor_L_aktiv(true);
  M_L.setMaxSpeed(M_LR_MaxSpeed); // stp/s
  M_L.setAcceleration(M_LR_Acceleration); // stp/s^2

  Schrittmotor_R_aktiv(true);
  M_R.setMaxSpeed(M_LR_MaxSpeed); // stp/s
  M_R.setAcceleration(M_LR_Acceleration); // stp/s^2

  int Drehrichtung = random(0, 10);
  if (Drehrichtung < 2)
  {
    Drehrichtung = 1;
  }
  else
  {
    Drehrichtung = -1;
  }
  if ((AddColShovel_R != 0) and (AddColShovel_L != 0))
  {
    TM1637_actionHappend_3333(); // Anzeigen was passiert ist
  }
  else if (AddColShovel_L != 0)
  {
    TM1637_actionHappend_2222(); // Anzeigen was passiert ist
  }
  else if (AddColShovel_R != 0)
  {
    TM1637_actionHappend_1111(); // Anzeigen was passiert ist
  }


  M_L.setTargetRel(Drehrichtung * AddColShovel_L * motorsteps * gear_ratio * M_LR_microstepping / Anzahl_ColShovel);
  M_R.setTargetRel(Drehrichtung * AddColShovel_R * motorsteps * gear_ratio * M_LR_microstepping / Anzahl_ColShovel);

  StepController.moveAsync(M_L, M_R);
}

void Stepper_loopMetronomeColor()
{
  if (ColorMixingMetronomeColor == true)
  {
    // Einmalig Color shift
    if ((ColorTimeSeconds_shift > 0) and (ColorTimeSeconds_R > 0))
    {
      Chrono_ColorTimeSeconds_R.add(ColorTimeSeconds_shift * 1000);
      ColorTimeSeconds_shift = 0;
    }

    // Intervall passed Farbe L
    if ( Chrono_ColorTimeSeconds_L.hasPassed(ColorTimeSeconds_L * 1000) )
    {
      Chrono_ColorTimeSeconds_L.restart();
      // Motor L
      ColorMixing_AddColor(1, 0);
    }
    // Intervall passed Farbe R
    if ( Chrono_ColorTimeSeconds_R.hasPassed(ColorTimeSeconds_R * 1000) )
    {
      Chrono_ColorTimeSeconds_R.restart();
      // Motor R
      ColorMixing_AddColor(0, 1);
    }
  }
}

void Chrono_MetronomeColorRestart_L()
{
  Chrono_ColorTimeSeconds_L.restart();
}

void Chrono_MetronomeColorRestart_R()
{
  Chrono_ColorTimeSeconds_R.restart();
}

void Chrono_MetronomeColorStop_L()
{
  Chrono_ColorTimeSeconds_L.stop();
}

void Chrono_MetronomeColorStop_R()
{
  Chrono_ColorTimeSeconds_R.stop();
}

void Stepper_loopWatchdogDisableSteppers()
{
  if ( Chrono_StepperWatchdog.hasPassed(2000) )
  {
    Chrono_StepperWatchdog.restart();
    // Turn Steppers off
    if (StepController.isRunning() == false)
    {
      Schrittmotor_L_aktiv(false);
      Schrittmotor_R_aktiv(false);
    }
  }
}
