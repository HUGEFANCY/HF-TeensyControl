
// Define the digital inputs
#define jB1 1  // Joystick button 1
#define jB2 0  // Joystick button 2
#define t1 7   // Toggle switch 1
#define t2 4   // Toggle switch 1
#define b1 8   // Button 1
#define b2 9   // Button 2
#define b3 2   // Button 3
#define b4 3   // Button 4

struct DataPackageOutcoming // Max size of this struct is 32 bytes - NRF24L01 buffer limit
{
  byte j1PotX;
  byte j1PotY;
  byte j1Button;
  byte j2PotX;
  byte j2PotY;
  byte j2Button;
  byte pot1;
  byte pot2;
  byte tSwitch1;
  byte tSwitch2;
  byte button1;
  byte button2;
  byte button3;
  byte button4;
};
DataPackageOutcoming dataOutgoing; // Create a variable with the above structure

void setup_Inputs()
{

  // Activate the Arduino internal pull-up resistors
  pinMode(jB1, INPUT_PULLUP);
  pinMode(jB2, INPUT_PULLUP);
  pinMode(t1, INPUT_PULLUP);
  pinMode(t2, INPUT_PULLUP);
  pinMode(b1, INPUT_PULLUP);
  pinMode(b2, INPUT_PULLUP);
  pinMode(b3, INPUT_PULLUP);
  pinMode(b4, INPUT_PULLUP);

  // Set initial default values
  dataOutgoing.j1PotX = 127; // Values from 0 to 255. When Joystick is in resting position, the value is in the middle, or 127. We actually map the pot value from 0 to 1023 to 0 to 255 because that's one BYTE value
  dataOutgoing.j1PotY = 127;
  dataOutgoing.j2PotX = 127;
  dataOutgoing.j2PotY = 127;
  dataOutgoing.j1Button = 1;
  dataOutgoing.j2Button = 1;
  dataOutgoing.pot1 = 1;
  dataOutgoing.pot2 = 1;
  dataOutgoing.tSwitch1 = 1;
  dataOutgoing.tSwitch2 = 1;
  dataOutgoing.button1 = 1;
  dataOutgoing.button2 = 1;
  dataOutgoing.button3 = 1;
  dataOutgoing.button4 = 1;
}

void loop_Inputs()
{
  // Read all analog inputs and map them to one Byte value
  dataOutgoing.j1PotX = map(analogRead(A1), 0, 1023, 0, 255); // Convert the analog read value from 0 to 1023 into a BYTE value from 0 to 255
  dataOutgoing.j1PotY = map(analogRead(A0), 0, 1023, 0, 255);
  dataOutgoing.j2PotX = map(analogRead(A2), 0, 1023, 0, 255);
  dataOutgoing.j2PotY = map(analogRead(A3), 0, 1023, 0, 255);
  dataOutgoing.pot1 = map(analogRead(A7), 0, 1023, 0, 255);
  dataOutgoing.pot2 = map(analogRead(A6), 0, 1023, 0, 255);
  // Read all digital inputs
  dataOutgoing.j1Button = digitalRead(jB1);
  dataOutgoing.j2Button = digitalRead(jB2);
  dataOutgoing.tSwitch1 = digitalRead(t1);
  dataOutgoing.tSwitch2 = digitalRead(t2);
  dataOutgoing.button1 = digitalRead(b1);
  dataOutgoing.button2 = digitalRead(b2);
  dataOutgoing.button3 = digitalRead(b3);
  dataOutgoing.button4 = digitalRead(b4);
}