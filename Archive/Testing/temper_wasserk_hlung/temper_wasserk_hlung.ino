int sensorPin = A8;
int bitValueNTC = 0;
long resistance1 = 100000;                 //Ohm
int bValue = 3435;                          // B- Wert vom NTC
double resistanceNTC = 0;
double kelvintemp = 273.15;                // 0°Celsius in Kelvin
double Tn = kelvintemp + 25;               //Nenntemperatur in Kelvin
double TKelvin = 0;                        //Die errechnete Isttemperatur
double T = 0;                              //Die errechnete Isttemperatur

void setup() {

  Serial.begin(9600);
}

void loop() {

    Serial.println("Sensormessung:  ");
    bitValueNTC = analogRead(sensorPin);      // lese Analogwert an A0 aus
    resistanceNTC = resistance1 * (((double)bitValueNTC / 1024) / (1 - ((double)bitValueNTC / 1024)));
    // berechne den resistanceswert vom NTC
    TKelvin = 1 / ((1 / Tn) + ((double)1 / bValue) * log((double)resistanceNTC / resistance1));
    // ermittle die Temperatur in Kelvin
    T = TKelvin - kelvintemp;                // ermittle die Temperatur in °C

    Serial.println("Analog: ");              //
    Serial.println(bitValueNTC);              //
    Serial.println("NTC- resistance: ");     //Gebe die ermittelten Werte aus
    Serial.println(resistanceNTC);           //
    Serial.println("Temperatur: ");          //Gebe die ermittelten Werte aus
    Serial.println(T);                       //

    delay(1000);                               // Warte eine Sekunde und mache alles nochmal
  
}
