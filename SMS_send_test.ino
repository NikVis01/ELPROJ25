// This is a test code snippet gathered from https://www.manelsoft.com/projects/arduino_sim800.aspx
// It describes how to send an SMS using a SIM800 module.

// NOTE: THIS CODE IS FOR ARDUINO UNO FOR NANO IT MIGHT LOOK A LITTLE DIFFERENT

#include <SoftwareSerial.h>

SoftwareSerial gsmSerial(7, 8);

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
  gsmSerial.println("AT+CMGS=\"+94123456789\""); //Your mobile number with country code
  updateSerial();
  gsmSerial.print("Hello, Regards from SIM800"); //Your message
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

                    
