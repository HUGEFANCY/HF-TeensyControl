// Code that sets the PWM values for the print-cooling fans

byte pwmCoolingPin = 30;

void CoolingPWM_setup()
{
  pinMode(pwmCoolingPin, OUTPUT); // Set corresponding pin as output
}

void CoolingPWM()
{
  int PWMvalue = map(PwmValuePartCoolingFanMarlin, 0, 255, 255, 0); // Invert PWM value
  analogWrite(pwmCoolingPin, PWMvalue);
  //Serial.print("PWMvalue = "); Serial.println(PwmValuePartCoolingFanMarlin);
}
