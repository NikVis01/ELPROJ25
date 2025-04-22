#include <SoftwareSerial.h>

// Defining Rx and Tx pins
const byte TxPin = 9;
const byte RxPin = 2;

// Defining reset and sleep mode pins
// const byte RstPin = 7; // Reset, pull LOW for hard reset
// const byte DtrPin = 4; // Sleep mode, pull HIGH to enter sleepmode and stop serial coms, pull LOW for 50 ms to exit sleepmode

// Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(TxPin, RxPin);  //SIM800L Tx & Rx is connected to Arduino D9 & D2

void setup() {
  // Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);

  // Setting reset and sleep to proper high/low to turn on module
  // pinMode(RstPin, OUTPUT);
  // pinMode(DtrPin, OUTPUT);

  // digitalWrite(RstPin, LOW); // Hard reset of the module at the beginning
  // digitalWrite(DtrPin, LOW); // Wakey wakey eggs and bakey


  // Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);

  Serial.println("Initializing...");
  delay(1000);

  Serial.println("Starting AT handshake");
  
  mySerial.println("AT"); // If hanshake successfull should return OK
  updateSerial();
  mySerial.println("AT+CSQ"); // Signal quality test, value range is 0-31 , 31 is the best
  updateSerial();
  mySerial.println("AT+CCID"); // Read SIM information to confirm whether the SIM is plugged
  updateSerial();
  mySerial.println("AT+CREG?"); //Check whether it has registered in the network
  updateSerial();
}

void loop() {
  updateSerial();
}

void updateSerial() {
  delay(500);
  while (Serial.available()) {
    mySerial.write(Serial.read()); // Forward what Serial received to Software Serial Port
  }
  while (mySerial.available()) {
    Serial.write(mySerial.read()); // Forward what Software Serial received to Serial Port
  }
}
