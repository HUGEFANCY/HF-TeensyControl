//TODO refactor:
//    - confusing variable names 
//    - redundant comments 
//    - 
// Temperature sensing for the watercooling system
// Pinout cooling-fan radiator: https://alienware-forum.de/index.php/Attachment/1319-6b70473c-Diagram-1-png/?s=91c43ccfd0541d71ad162c7ef674f13691a83f23

// Pinout
int PinWaterIn = A21;    //Pin that is connected to the thermistor on the water inlet
int PinWaterOut = A22;   //Pin that is connected to the thermistor on the water outlet

Chrono chronoTempWaterInterval;

int ReadWaterTemp(int Pin)
{
  const int bValue = 3435; // B- value from NTC
  const long Resistance = 10000; // Ohm
  double resistanceNTC = 0; // Will be calculated

  int bitValueNTC = 0;

  const double kelvintemp = 273.15; // 0° Celsius in Kelvin
  double Tn = kelvintemp + 25; // nominal temperature in Kelvin
  double TKelvin = 0; // Calculated current tempereature in Kelvin
  int T = 0; // Calculated current temperature in °C

  bitValueNTC = analogRead(PinWaterIn);
  resistanceNTC = Resistance * (((double)bitValueNTC / 1024) / (1 - ((double)bitValueNTC / 1024))); // berechne den resistanceswert vom NTC

  TKelvin = 1 / ((1 / Tn) + ((double)1 / bValue) * log((double)resistanceNTC / Resistance)); // Calculate current temperature in °K
  T = round(TKelvin - kelvintemp); // convert from K to °C
  //Serial.println(T);
  return (T);

}

void TempWater_loop()
{
  if ( chronoTempWaterInterval.hasPassed(1000) )  // check if the metro has passed its interval
  {
    chronoTempWaterInterval.restart();
    TempWatercooling_In = ReadWaterTemp(PinWaterIn);
    TempWatercooling_Out = ReadWaterTemp(PinWaterOut);
  }
}
