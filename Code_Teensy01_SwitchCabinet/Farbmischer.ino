
// Magic for ColorMixing


byte Farbschaufel(byte Motor_L, byte Motor_R) // max 4 Bit = 15 Schauffeln pro Aufruf
{
  byte ColorMixingRevolutions = 0; // Bit 1-4 -> Motor Left, Bit 5-8 -> Motor Right, all together max 8 Bit = 255
  //  (ColorMixingRevolutions & 0b11110000) >> 4 = Motor_L; // get the first 4 bits of the byte and shift them right 4 places
  // (ColorMixingRevolutions & 0b00001111) = Motor_R; // get the last 4 bits of the byte

  return ColorMixingRevolutions; // Bit 1-4 -> Motor Left, Bit 5-8 -> Motor Right, all together max 8 Bit = 255
}
