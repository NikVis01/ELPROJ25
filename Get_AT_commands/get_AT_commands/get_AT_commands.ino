#include <SoftwareSerial.h>

////// ------- Defining Rx and Tx pins ------- 

const byte TxPin = 2; // Arduino Tx --> SIM800L RxD 
const byte RxPin = 9; // SIM800L TxD --> Arduino Rx
const byte RstPin = 7; // Reset, pull LOW for hard reset
const byte DtrPin = 4; // Sleep mode, pull HIGH to enter sleepmode, pull LOW for 50 ms to exit sleepmode

////// ------- Create software serial object to communicate with SIM800L ------- 
SoftwareSerial sim800(TxPin, RxPin); // Tx = 2, Rx = 9 (ON THE ARDUINO NANO, flipped for inputs for the sim800L)

void setup() {
  // Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600); // Arduino to serial monitor

  // Setting reset and sleep to proper high/low to turn on module
  pinMode(RstPin, OUTPUT);
  pinMode(DtrPin, OUTPUT);

  Serial.println("Reseting SIM800L");
  digitalWrite(RstPin, LOW); // Hard reset of the module at the beginning
  delay(100);
  digitalWrite(RstPin, HIGH);
  
  Serial.println("Exiting sleepmode");
  // digitalWrite(DtrPin, HIGH);
  // delay(500);
  digitalWrite(DtrPin, LOW); // Wakey wakey eggs and bakey

  delay(1000);
  
  ////// ------- Begin serial communication with Arduino and SIM800L ------- 
  sim800.begin(9600); // was 9600 baud which translates to each bit being 104 microseconds wide

  sim800.println("ATE1"); // Making sure echo is on for debugging TURNS OUT THIS DOESNT FUCKING DO ANYTHING EVEN AMAZING

  Serial.println("Debug mode...");
  sim800.println("AT+CMEE=2");
  updateSerial();

  Serial.println("Scanning for network");
  sim800.println("AT+COPS=?");
  updateSerial();
  
  Serial.println("Checking firmware version and name");
  sim800.println("ATI"); // Checking firmware and name
  delay(500);
  updateSerial();
  
  Serial.println("Starting AT handshake");
  sim800.println("AT"); // If hanshake successfull should return OK
  delay(2000);
  updateSerial();

  Serial.println("Testing signal quality");
  sim800.println("AT+CSQ"); // Signal quality test, value range is 0-31 , 31 is the best
  delay(2000);
  updateSerial();

  Serial.println("Confirming SIM connection");
  sim800.println("AT+CCID"); // Read SIM information to confirm whether the SIM is plugged
  delay(2000);
  updateSerial();
  
  Serial.println("Checking network registration");
  sim800.println("AT+CREG?"); //Check whether it has registered in the network
  delay(2000);
  updateSerial();
}

void loop() {
  updateSerial();
}
void updateSerial() {
  delay(500);

  while (Serial.available()) {
    char inChar = Serial.read();
    sim800.write(inChar);
    Serial.print("→ SIM800L: "); Serial.println(inChar);
  }

  while (sim800.available()) {
    char outChar = sim800.read();
    Serial.print("← SIM800L: "); Serial.println(outChar);
  }
}
