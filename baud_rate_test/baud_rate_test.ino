#include <SoftwareSerial.h>

// Define SoftwareSerial pins
const int SIM800_RX = 9;  // Arduino receives from SIM800L
const int SIM800_TX = 2;  // Arduino sends to SIM800L
const int SIM800_DTR = 4; // DTR pin

SoftwareSerial sim800(SIM800_RX, SIM800_TX);

// List of baud rates to try
const int baudRates[] = {9600, 4800, 19200, 38400, 57600, 115200};
const int baudRateCount = sizeof(baudRates) / sizeof(baudRates[0]);

void setup() {
  Serial.begin(9600); // For PC Serial Monitor
  pinMode(SIM800_DTR, OUTPUT);
  digitalWrite(SIM800_DTR, LOW); // Keep module awake

  Serial.println("Starting auto-baud detection...");

  for (int i = 0; i < baudRateCount; i++) {
    int currentBaud = baudRates[i];

    Serial.print("Trying baud rate: ");
    Serial.println(currentBaud);

    sim800.begin(currentBaud);
    delay(500); // Small wait

    flushSerial(); // Clear junk if any

    // Send basic AT command
    sim800.println("AT");
    delay(500);

    if (checkOKResponse()) {
      Serial.println();
      Serial.print("✅ Found working baud rate: ");
      Serial.println(currentBaud);

      while (true) {
        // Stay here forever after finding
      }
    }
  }

  Serial.println("❌ No working baud rate found. Check wiring or power.");
}

void loop() {
  // Nothing here
}

// Utility: Read incoming Serial and look for "OK"
bool checkOKResponse() {
  unsigned long startTime = millis();
  String response = "";

  while (millis() - startTime < 2000) { // 2s timeout
    if (sim800.available()) {
      char c = sim800.read();
      response += c;
    }
  }

  response.trim();
  response.toUpperCase(); // just in case

  if (response.indexOf("OK") != -1) {
    return true;
  }
  return false;
}

// Utility: Flush any existing garbage
void flushSerial() {
  while (sim800.available()) {
    sim800.read();
  }
}
