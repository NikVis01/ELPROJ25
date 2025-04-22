// This is a test code snippet altered from https://www.manelsoft.com/projects/arduino_sim800.aspx
// It describes how to send an SMS using a SIM800 module.

#include <SoftwareSerial.h>

// Defining Rx and Tx pins
const byte TxPin = 9;
const byte RxPin = 2;

// Defining reset and sleep mode pins
// const byte RstPin = 7; // Reset, pull LOW for hard reset
// const byte DtrPin = 4; // Sleep mode, pull HIGH to enter sleepmode and stop serial coms, pull LOW for 50 ms to exit sleepmode


SoftwareSerial gsmSerial(TxPin, RxPin);

void setup()
{
  //Start serial communication
  Serial.begin(9600);
  gsmSerial.begin(9600);

  Serial.println("Starting..");
  delay(1000);

  gsmSerial.println("AT");
  updateSerial();
  gsmSerial.println("AT+CMGF=1");// Change to text mode
  updateSerial();
  gsmSerial.println("AT+CMGS=\"+46793134464\""); //Your mobile number with country code
  updateSerial();
  gsmSerial.print("Big balling"); //Your message
  updateSerial();
  gsmSerial.write(26);
}

void loop()
{
  //Nothing to do with the loop in this code
}

//Method for serial communications
void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    gsmSerial.write(Serial.read());
  }
  while(gsmSerial.available()) 
  {
    Serial.write(gsmSerial.read());
  }
}

                    
