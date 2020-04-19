// RS485 long distance communication betweent Teensy Schaltschrank (Master) and Teensy Extruder (Slave) über UART with selfmade Communication Protocol
// Anleitung RS485 UART: https://circuitdigest.com/microcontroller-projects/rs485-serial-communication-between-arduino-uno-and-arduino-nano
// Anleitung designing a communication protocol: https://henryforceblog.wordpress.com/2015/03/12/designing-a-communication-protocol-using-arduinos-serial-library/
// Modbus (not used) https://industruino.com/blog/our-news-1/post/modbus-rtu-master-and-slave-14

// Master Devive

// PINOUT
// R0 -> Teensy Rx1 / PIN 0
// RE = DE -> Teensy PIN 8
// DI -> Teensy Tx1 / PIN 1
// VCC -> 5V
// A -> Kabelterminal 17
// B -> Kabelterminal 18
// GND -> Teensy GND

const byte RS485_enablePin = 8;

const uint8_t bufferSize = 5; // Buffer communication protocoll
uint8_t buffer[bufferSize];
uint8_t readCounter;
uint8_t isHeader;
uint8_t firstTimeHeader; // Flag that helps us restart counter when we first find header byte


const uint8_t header_AnswerUpdateVariables = 0x7C; // Bufferheader: Aktion Farbmischer


void RS485_setup()
{
  while (!Serial1);
  Serial1.begin(9600); // Serial1 für RS485

  pinMode(RS485_enablePin, OUTPUT);
  delay(10);
  digitalWrite(RS485_enablePin, HIGH);  // always high as Master Writes data to Slave
}

void RS485_SentUpdateVaribles()
{
  // Byte 0 Header (0x7E)
  // Byte 1 targetTempExtruderMarlin Byte 01
  // Byte 2 targetTempExtruderMarlin Byte 02
  // Byte 3 pwmValuePartCoolingFanMarlin
  // Byte 4 Checksum

  // 10 bit max value is 1023 (analogRead() returns a 10 bit value) // Divide our 10 bit value by 4, to obtain and effective 8 bit value
  // 9 bit max value is 511, // Divide our 9 bit value by 2, to obtain and effective 8 bit value
  // 8 bit max value is 255.

  buffer[0] = 0x7E; // Bufferheader: Verkündung Teensy Schaltschrank Update Variablen

  if (targetTempExtruderMarlin <= 255)
  {
    buffer[1] = targetTempExtruderMarlin; // Wert von 0-255°C
    buffer[2] = 0;
  }
  else if ((targetTempExtruderMarlin > 255) and (targetTempExtruderMarlin <= 510))
  {
    buffer[1] = 255;
    buffer[2] = targetTempExtruderMarlin - 255; // Wert von 256-510°C
  }

  buffer[3] = PwmValuePartCoolingFanMarlin;

  // ### ToDo Bitshift, da schneller

  buffer[4] = checksum();

  Serial1.write(buffer, bufferSize); // We send all bytes stored in the buffer

  RS485_WaitForAnswer();
}



void RS485_WaitForAnswer()
{
  digitalWrite(RS485_enablePin, LOW);  // turn RS485 module LOW as Slave device
  delay(100);
  
  if (Serial1.available() > 0) // Check if there is any data available to read
  {
    uint8_t c = Serial1.read(); // read only one byte at a time

    if (c == header_AnswerUpdateVariables) // Check if header is found
    {
      if (!firstTimeHeader)
      {
        isHeader = 1;
        readCounter = 0;
        firstTimeHeader = 1;
      }
    }
    buffer[readCounter] = c; // store received byte, increase readCounter ### FEHLER???
    readCounter++;

    if (readCounter >= bufferSize) // prior overflow, we have to restart readCounter
    {
      readCounter = 0;

      if (isHeader) // if header was found
      {
        uint8_t checksumValue = buffer[4]; // get checksum value from buffer's last value, according to defined protocol
        if (verifyChecksum(checksumValue)) // perform checksum validation, it's optional but really suggested
        {
          // Folgende Antwort erhalten:

          // Byte 0 Header (0x7C)
          // Byte 1 RealTempExtruderForMarlin_01 Byte 01
          // Byte 2 RealTempExtruderForMarlin_02 Byte 02
          // Byte 3 leer
          // Byte 4 Checksum

          // Update globale Variablen
          RealTempExtruderForMarlin = buffer[1] + buffer[2]; // gesendete 8 Bit Werte wiedeer auf die ursprünglichen 9 Bit zurückführen
          // buffer[3] noch frei
          RS485_AnswerUpdateVariables_LastUpdatePreviousMillis = currentMillis; // für Timeout falls wir lange nichts mehr vom Teensy Extruder gehört haben
        }
        // restart header flag
        isHeader = 0;
        firstTimeHeader = 0;
      }
    }
  }
  digitalWrite(RS485_enablePin, HIGH);  // always high as Master Writes data to Slave (normal mode Master)
}


void RS485_SentFarbmischerGibSchaufeln(byte SchaufelnMotor_L, byte SchaufelnMotor_R)
{
  // Byte 0 Header (0x7D)
  // Byte 1 Schaufeln Links
  // Byte 2 Schaufeln Rechts
  // Byte 3 frei
  // Byte 4 Checksum

  buffer[0] = 0x7D; // Bufferheader: Aktion Farbmischer
  buffer[1] = SchaufelnMotor_L;
  buffer[2] = SchaufelnMotor_R;
  buffer[3] = 0; // noch frei

  buffer[4] = checksum();

  Serial1.write(buffer, bufferSize); // We send all bytes stored in the buffer

  delay(100);
}

//We perform a sum of all bytes, except the one that corresponds to the original checksum value. After summing we need to AND the result to a byte value.
uint8_t checksum() {
  uint8_t result = 0;
  uint16_t sum = 0;

  for (uint8_t i = 0; i < (bufferSize - 1); i++) {
    sum += buffer[i];
  }
  result = sum & 0xFF;

  return result;
}

// This a common checksum validation method. We perform a sum of all bytes, except the one that corresponds to the original checksum value. After summing we need to AND the result to a byte value.
uint8_t verifyChecksum(uint8_t originalResult)
{
  uint8_t result = 0;
  uint16_t sum = 0;

  for (uint8_t i = 0; i < (bufferSize - 1); i++)
  {
    sum += buffer[i];
  }
  result = sum & 0xFF;

  if (originalResult == result)
  {
    return 1;
  } else
  {
    return 0;
  }
}
