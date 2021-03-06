
// RS485 long distance communication betweent Teensy_Switchcabinet (Master) and Teensy_Extruder (Slave) via UART with selfmade Communication Protocol
// Tutorial RS485 UART: https://circuitdigest.com/microcontroller-projects/rs485-serial-communication-between-arduino-uno-and-arduino-nano
// Tutorial designing a communication protocol: https://henryforceblog.wordpress.com/2015/03/12/designing-a-communication-protocol-using-arduinos-serial-library/

// Slave Device

// PINOUT
// R0 -> Teensy Rx1 / PIN 0
// RE = DE -> Teensy PIN 8
// DI -> Teensy Tx1 / PIN 1
// VCC -> 5V
// A -> Kabelterminal 17
// B -> Kabelterminal 18
// GND -> Teensy GND

const byte RS485_enablePin = 13;

// Send
const uint8_t bufferSizeRS485 = 5;
uint8_t bufferRS485[bufferSizeRS485];
// Receive
uint8_t readCounter = 0;
uint8_t isHeader = 0;
uint8_t firstTimeHeader = 0; // Flag that helps us restart counter when we first find header byte

// headers
const uint8_t header_AbsenderExtruder_Statusupdate = 13;
const uint8_t header_AbsenderSwitchcabinet_Statusupdate = 14;
const uint8_t header_AbsenderSwitchcabinet_clickColor = 15;
const uint8_t header_AbsenderSwitchcabinet_metronomeColor = 16;


void RS485_setup()
{
  while (!Serial1);
  Serial1.begin(250000); // Serial1 für RS485
  Serial1.transmitterEnable(RS485_enablePin);
  delay(10);
}


//We perform a sum of all bytes, except the one that corresponds to the original checksum value. After summing we need to AND the result to a byte value.
uint8_t checksum() {
  uint8_t result = 0;
  uint16_t sum = 0;

  for (uint8_t i = 0; i < (bufferSizeRS485 - 1); i++)
  {
    sum += bufferRS485[i];
  }
  sum = sum + 10;
  result = sum & 0xFF;

  return result;
}

// This a common checksum validation method. We perform a sum of all bytes, except the one that corresponds to the original checksum value. After summing we need to AND the result to a byte value.
uint8_t verifyChecksum(uint8_t originalResult)
{
  uint8_t result = 0;
  uint16_t sum = 0;

  for (uint8_t i = 0; i < (bufferSizeRS485 - 1); i++)
  {
    sum += bufferRS485[i];
  }
  sum = sum + 10;
  result = sum & 0xFF;

  if (originalResult == result)
  {
    return 1;
  } else
  {
    return 0;
  }
}


void RS485_Extruder_CheckIfUpdateAvalible()
{
  int BreakCounter = 0;
  while (BreakCounter <= 2)
  {
    if (Serial1.available() > 0) // Check if there is any data available to read
    {
      uint8_t c = Serial1.read(); // read only one byte at a time

      if ((c == header_AbsenderSwitchcabinet_Statusupdate) or (c == header_AbsenderSwitchcabinet_clickColor) or (c == header_AbsenderSwitchcabinet_metronomeColor)) // Check if header is found
      {
        // We must consider that we may sometimes receive unformatted data, and given the case we must ignore it and restart our reading code.
        // If it's the first time we find the header, we restart readCounter indicating that data is coming.
        // It's possible the header appears again as a data byte. That's why this conditional is implemented, so that we don't restart readCounter and corrupt the data.
        if (!firstTimeHeader)
        {
          isHeader = true;
          readCounter = 0;
          firstTimeHeader = true;
        }
      }
      bufferRS485[readCounter] = c; // store received byte, increase readCounter
      //Serial.print("counter = "); Serial.print(readCounter); Serial.print(" // "); Serial.println(bufferRS485[readCounter]);
      readCounter++;

      if (readCounter >= bufferSizeRS485) // prior overflow, we have to restart readCounter
      {
        readCounter = 0;

        if (isHeader == 1) // if header was found
        {
          uint8_t checksumValue = bufferRS485[4]; // get checksum value from buffer's last value, according to defined protocol
          if (verifyChecksum(checksumValue) == true) // perform checksum validation, it's optional but really suggested
          {
            if (bufferRS485[0] == header_AbsenderSwitchcabinet_Statusupdate)
            {
              // Byte 0 Header
              // Byte 1 TargetTempExtruderMarlin Byte 01
              // Byte 2 TargetTempExtruderMarlin Byte 02
              // Byte 3 pwmValuePartCoolingFanMarlin
              // Byte 4 Checksum

              TargetTempExtruderMarlin = bufferRS485[1] + bufferRS485[2]; // gesendete 8 Bit Werte wiedeer auf die ursprünglichen 9 Bit zurückführen
              Serial.print("Receiving from Switchcabinet: TargetTempExtruderMarlin = "); Serial.println(TargetTempExtruderMarlin);
              PwmValuePartCoolingFanMarlin = bufferRS485[3];
              CoolingPWM(); // Kühlung aktuallisieren

              // Timeout Verbindung Switchcabinet – Extruder weggebrochen
              RS485_updateVariables_LastUpdatePreviousMillis = currentMillis; // für Timeout falls wir lange nichts mehr vom Teensy Switchcabinet gehört haben

              // Antwort und sende das eigene Statusupdate
              RS485_Extruder_Send_Statusupdate();
            }

            else if (bufferRS485[0] == header_AbsenderSwitchcabinet_clickColor)
            {
              // Byte 0 Header
              // Byte 1 ColShovel Left
              // Byte 2 ColShovel Right
              // Byte 3 N/A
              // Byte 4 Checksum
              byte ColShovel_L = bufferRS485[1];
              byte ColShovel_R = bufferRS485[2];
              
              if (ColShovel_L != 0)
              {

                ColorMixing_AddColor(1, 0);
              }
              if (ColShovel_R != 0)
              {
               
                ColorMixing_AddColor(0, 1);
              }
            }

            else if (bufferRS485[0] == header_AbsenderSwitchcabinet_metronomeColor)
            {
              // Byte 0 Header
              // Byte 1 ColorTime255_L
              // Byte 2 ColorTime255_R
              // Byte 3 ColorTime255_shift
              // Byte 4 Checksum
              ColorTimeSeconds_L = map(bufferRS485[1], 0, 255, 0, 600);
              ColorTimeSeconds_R = map(bufferRS485[2], 0, 255, 0, 600);
              ColorTimeSeconds_shift = map(bufferRS485[3], 0, 255, 0, 600);
              Serial.print("ColorTimeSeconds_L = ");Serial.println(ColorTimeSeconds_L);
              Serial.print("ColorTimeSeconds_R = ");Serial.println(ColorTimeSeconds_R);
              Serial.print("ColorTimeSeconds_shift = ");Serial.println(ColorTimeSeconds_shift);

              // LINKS
              if ((ColorTimeSeconds_L != 0))
              {
                ColorMixingMetronomeColor = true;
                Chrono_MetronomeColorRestart_L();
              }
              else
              {
                Chrono_MetronomeColorStop_L();
              }

              // RECHTS
              if ((ColorTimeSeconds_R != 0))
              {
                ColorMixingMetronomeColor = true;
                Chrono_MetronomeColorRestart_R();
              }
              else
              {
                Chrono_MetronomeColorStop_R();
              }
              
              // alles stoppen
              if ((ColorTimeSeconds_L == 0) and (ColorTimeSeconds_R == 0))
              {
                ColorMixingMetronomeColor = false;
                Chrono_MetronomeColorStop_L();
                Chrono_MetronomeColorStop_R();
              }

              //TM1637_actionHappend_8888(); // Show what happened
            }
          }
          // restart header flag
          isHeader = 0;
          firstTimeHeader = 0;
        }
      }
      BreakCounter++;
      //delay(5);
    }
  }
  //Serial.println("Break Loop");
}

void RS485_Extruder_Send_Statusupdate()
{


  // Byte 0 Header
  // Byte 1 CombinedRealTempExtruder Byte 01
  // Byte 2 CombinedRealTempExtruder Byte 02
  // Byte 3 leer
  // Byte 4 Checksum

  //Serial.print("CombinedRealTempExtruder = ");Serial.println(CombinedRealTempExtruder);

  bufferRS485[0] = header_AbsenderExtruder_Statusupdate; // Bufferheader
  if (CombinedRealTempExtruder <= 255)
  {
    bufferRS485[1] = CombinedRealTempExtruder; // Wert von 0-255°C
    bufferRS485[2] = 0;
  }
  else if ((CombinedRealTempExtruder > 255) and (CombinedRealTempExtruder <= 510))
  {
    bufferRS485[1] = 255;
    bufferRS485[2] = CombinedRealTempExtruder - 255; // Wert von 256-510°C
  }
  bufferRS485[3] = 0;
  bufferRS485[4] = checksum();

  // Senden
  Serial1.write(bufferRS485, bufferSizeRS485);
  RGB_Lila();
  Serial.println("gesende Statusupdate an Switchcabinet");
  RGB_Off();
}
