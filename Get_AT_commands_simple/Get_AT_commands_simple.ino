#include <SoftwareSerial.h>

const int SIM800_RX = 9;  // SIM800L TX --> Arduino Nano D9
const int SIM800_TX = 2;  // Arduino Nano D2 --> SIM800L RX
const int SIM800_DTR = 4; // DTR pin

SoftwareSerial sim800(SIM800_RX, SIM800_TX); // SoftwareSerial(rx, tx)

void setup() {
  pinMode(SIM800_DTR, OUTPUT);
  digitalWrite(SIM800_DTR, LOW); // Keep awake

  Serial.begin(9600);  // For Serial Monitor
  sim800.begin(19200);  // SIM800L Default (usually)

  delay(5000); // Let SIM800L boot

  Serial.println("Type AT commands:");
}

void loop() {
  if (Serial.available()) {
    sim800.write(Serial.read());
  }
  if (sim800.available()) {
    Serial.write(sim800.read());
  }
}
