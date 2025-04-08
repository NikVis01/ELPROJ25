// This is a test code snippet gathered from https://www.manelsoft.com/projects/arduino_sim800.aspx 
// It describes how to get the AT commands from a SIM800 module.

//SIM800L Tx & Rx is connected to Arduino 7 & 8
SoftwareSerial gsmSerial(7, 8); 

void setup()
{
  //Start serial communication
  Serial.begin(9600);
  gsmSerial.begin(9600);

  Serial.println("Initializing...");
  delay(1000);

  gsmSerial.println("AT"); //Basic AT command
  updateSerial();
  gsmSerial.println("AT+CSQ"); //GSM signal strength. 0-31 , 31 is the best
  updateSerial();
  gsmSerial.println("AT+CCID"); //Read SIM card information
  updateSerial();
  gsmSerial.println("AT+CREG?"); //Check if the module is registered to a network
  updateSerial();
  gsmSerial.println("AT+CBC"); //Charging status and remaining battery capacity
  updateSerial();
  gsmSerial.println("AT+GSV"); //Product information
  updateSerial();
}

void loop()
{
  updateSerial();
}

//Method for serial communication
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

                    