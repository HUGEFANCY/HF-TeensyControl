/*
  Arduino Wireless Network - Multiple NRF24L01 Tutorial
          == Base/ Master Node 00==
  by Dejan, www.HowToMechatronics.com
  Libraries:
  nRF24/RF24, https://github.com/nRF24/RF24
  nRF24/RF24Network, https://github.com/nRF24/RF24Network
*/

#include <SPI.h>

#include <RF24Network.h>
#include "RF24.h"

RF24 radio(5, 6); // CE, CSN -> Arduino Pro Mini Board 5, 6
RF24Network network(radio);

const int FunkChannel = 90;
const uint16_t FunkMasterSwitchcabinet = 00; // Address of the other node in Octal format // Switchcabinet, Master
const uint16_t FunkSlaveJoystick = 01; // Joystick, Slave

Metro Metro_FunkCheck = Metro(500);


struct DataPackageIncomming // Max size of this struct is 32 bytes - NRF24L01 buffer limit
{
  byte header = 0;
  byte val1 = 0; // TargetTemperatureZone_1 (laut Teensy Schatschrank)
  byte val2 = 0; // TargetTemperatureZone_2 (laut Teensy Schatschrank)
  byte val3 = 0; // RealTemperatureZone_1
  byte val4 = 0; // RealTemperatureZone_2
};
DataPackageIncomming dataIncoming; // Create a variable with the above structure



struct DataPackageOutgoing // Max size of this struct is 32 bytes - NRF24L01 buffer limit
{
  byte header = 0; // 1 = Temp                        // 2 = Joystick
  byte val1 = 0;   // NewTargetTemp_Zone1             // j1PotX;
  byte val2 = 0;   // NewTargetTemp_Zone2             // j1PotY;
  byte val3 = 0;   // PwmValuePartCoolingFanMarlin    // j1Button;
  byte val4 = 0; // j2PotX;
  byte val5 = 0; // j2PotY;
  byte val6 = 0; // j2Button;
  byte val7 = 0; // pot1;
  byte val8 = 0; // pot2;
  byte val9 = 0; // tSwitch1;
  byte val10 = 0; // tSwitch2;
  byte val11 = 0; // button1;
  byte val12 = 0; // button2;
  byte val13 = 0; // button3;
  byte val14 = 0; // button4;
};
DataPackageOutgoing dataOutgoing; // Create a variable with the above structure


void setup_Funk()
{
  SPI.begin();
  radio.begin();
  network.begin(FunkChannel, FunkSlaveJoystick); //(channel, node address)
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.setAutoAck(1); // Ensure autoACK is enabled
  radio.setRetries(15, 15);
  // delay How long to wait between each retry, in multiples of 250us, max is 15. 0 means 250us, 15 means 4000us.
  //count How many retries before giving up, max 15
}

// PWM
boolean FunkData_Temp_PWM()
{
  // Send data:
  RF24NetworkHeader header(FunkMasterSwitchcabinet);   // Address where the data is going
  dataOutgoing.header = 1;
  //dataOutgoing.val1 = NewTargetTemp_Zone1;
  //dataOutgoing.val2 = NewTargetTemp_Zone2;
  dataOutgoing.val3 = PwmValuePartCoolingFanMarlin;
  boolean ok = network.write(header, &dataOutgoing, sizeof(dataOutgoing)); // Send the data
  //Serial.println("Funk DATA Temp!");
  if (ok == true)
  {
    Serial.println("FunkData_Temp_PWM");
    return true;
  }
  else
  {
    return false;
  }
}


// Click Color MODE
boolean FunkData_clickColor(int RadA, int RadB)
{
  // Send data:
  RF24NetworkHeader header(FunkMasterSwitchcabinet);   // Address where the data is going
  dataOutgoing.header = 2; // Farbrad
  dataOutgoing.val1 = RadA; // RAD A
  dataOutgoing.val2 = RadB; // RAD B
  dataOutgoing.val3 = 0; // NA
  bool ok = network.write(header, &dataOutgoing, sizeof(dataOutgoing)); // Send the data

  if (ok == true)
  {
    Serial.println("Funk DATA Color A!");
    return true;
  }
  else
  {
    return false;
  }
}



boolean FunkData_colorMetronome()
{
  // Send data:
  RF24NetworkHeader header(FunkMasterSwitchcabinet);   // Address where the data is going
  dataOutgoing.header = 3; // Farbrad colorMetronome
  dataOutgoing.val1 = ColorTime255_L; // RAD L
  dataOutgoing.val2 = ColorTime255_R; // RAD R
  dataOutgoing.val3 = ColorTime255_shift; // shift LR
  bool ok = network.write(header, &dataOutgoing, sizeof(dataOutgoing)); // Send the data
  if (ok == true)
  {
    Serial.println("Funk DATA Color Periodisch!");
    return true;
  }
  else
  {
    return false;
  }
}




void loop_FunkCheck()
{
  //===== Receiving =====//
  network.update();

  while (network.available()) // Is there any incoming data?
  {
    RF24NetworkHeader header(FunkMasterSwitchcabinet);
    network.read(header, &dataIncoming, sizeof(dataIncoming)); // Read the incoming data

    if ((header.from_node == FunkMasterSwitchcabinet) and (dataIncoming.header == 1))
    {
      //Serial.println("Switchcabinet hat was gefunkt");

      if ((dataIncoming.val1 != TargetTemperatureZone_1) and (NewTargetTempAvalible_Zone1 == false))
      {
        TargetTemperatureZone_1 = dataIncoming.val1;
      }
      if ((dataIncoming.val2 != TargetTemperatureZone_2) and (NewTargetTempAvalible_Zone2 == false))
      {
        TargetTemperatureZone_2 = dataIncoming.val2;
      }
      if (dataIncoming.val3 != RealTemperatureZone_1)
      {
        RealTemperatureZone_1 = dataIncoming.val3;
      }
      if (dataIncoming.val4 != RealTemperatureZone_2)
      {
        RealTemperatureZone_2 = dataIncoming.val4;
      }
    }
  }
}
