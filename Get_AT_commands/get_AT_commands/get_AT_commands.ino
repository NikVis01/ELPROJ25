#include <SoftwareSerial.h>

////// ------- Defining Rx and Tx pins ------- 

const byte TxPin = 2; // Arduino Tx --> SIM800L RxD 
const byte RxPin = 9; // SIM800L TxD --> Arduino Rx
const byte RstPin = 7; // Reset, pull LOW for hard reset
const byte DtrPin = 4; // Sleep mode, pull HIGH to enter sleepmode, pull LOW for 50 ms to exit sleepmode

////// ------- Create software serial object to communicate with SIM800L ------- 
SoftwareSerial mySerial(TxPin, RxPin); // Tx = 2, Rx = 9 (ON THE ARDUINO NANO, flipped for inputs for the sim800L)

void setup() {
  // Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600); // Arduino to serial monitor

  // Setting reset and sleep to proper high/low to turn on module
  pinMode(RstPin, OUTPUT);
  pinMode(DtrPin, OUTPUT);

  Serial.println("Reseting SIM800L");
  digitalWrite(RstPin, LOW); // Hard reset of the module at the beginning
  delay(500);
  digitalWrite(RstPin, HIGH);
  
  Serial.println("Toggling sleepmode");
  digitalWrite(DtrPin, HIGH);
  delay(500);
  digitalWrite(DtrPin, LOW); // Wakey wakey eggs and bakey

  delay(1000);
  
  ////// ------- Begin serial communication with Arduino and SIM800L ------- 
  mySerial.begin(4800); // was 9800 baud which translates to each bit being 104 microseconds wide, now its 208 ms

  mySerial.println("ATE1"); // Making sure echo is on for debugging

  Serial.println("Checking firmware version and name");
  mySerial.println("ATI"); // Checking firmware and name
  delay(500);
  updateSerial();
  
  Serial.println("Starting AT handshake");
  mySerial.println("AT\r\n"); // If hanshake successfull should return OK
  delay(2000);
  updateSerial();

  Serial.println("Testing signal quality");
  mySerial.println("AT+CSQ"); // Signal quality test, value range is 0-31 , 31 is the best
  delay(2000);
  updateSerial();

  Serial.println("Confirming SIM connection");
  mySerial.println("AT+CCID"); // Read SIM information to confirm whether the SIM is plugged
  delay(2000);
  updateSerial();
  
  Serial.println("Checking network registration");
  mySerial.println("AT+CREG?"); //Check whether it has registered in the network
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
    mySerial.write(inChar);
    Serial.print("→ SIM800L: "); Serial.println(inChar);
  }

  while (mySerial.available()) {
    char outChar = mySerial.read();
    Serial.print("← SIM800L: "); Serial.println(outChar);
  }
}
