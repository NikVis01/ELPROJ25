  // --- Sketch for continous sensor data logging and storage on-board the Arduino Nano in EEPROM (non-voletile memory)
// 1. Reads Turbidity and Temperature sensor outputs once a day
// 2. Stores the readings in memory
// 3. Sets limit for max days (1k bytes should be enough for 100+ days of sensor data)
// 4. Lets user retrieve sensor readings later by plugging in a laptop and running Retrieve.py (found on github)


//// Hardware Specs: ////

// Turbidity sensor: DFRobot Gravity turbidity sensor (analog pin 6 on nano)
// Thermometer: Elektrokit DS18B20 thermometer (digital pin 11 on nano), note circuit-wise there is a pull-up resistor. This is because the DS18B20 uses a one-wire system.
 
// --- Considerations: EEPROM cell will be worn down with time if written to every day, but once a day is well within safe margins (EEPROM is rated for 100k write-cycles per cell)

#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//// --- Defining global variables & Pins --- // 
const int TEMP_PIN = 11; // Digital Pin 11
const int TURBIDITY_PIN = A2; // Analog Pin 2
const unsigned long LOG_INTERVAL_MS = 10000; // Log every 60 seconds (adjust as needed)

// --- Initialize sensors --- //
OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);

//// --- Memory allocation & Runtime state variables --- ////
// Each reading takes 8 bytes: 4 bytes for timestamp, 2 bytes for temperature, 2 bytes for turbidity
// Runtime variables are stored at the end of EEPROM
const int MAX_READINGS = 125; // 1000 bytes of EEPROM / 8 bytes per reading
const int EEPROM_SIZE = 1024; // Total EEPROM size
const int RUNTIME_VARS_START = EEPROM_SIZE - 2; // Start of runtime variable (current_index)

unsigned long last_log_time = 0;

void setup() {
  Serial.begin(9600);
  sensors.begin();

  while (!Serial);  // Wait for Serial if connected

  delay(1000); // Give time for the serial monitor to open
  
  // Initialize current_index if not already set
  int current_index;
  EEPROM.get(RUNTIME_VARS_START, current_index);

  if (current_index < 0 || current_index >= MAX_READINGS) {
    current_index = 0;
    EEPROM.put(RUNTIME_VARS_START, current_index);
  }

  delay(1000); // Give time for the serial monitor to open
  
}

void loop() {
  unsigned long now = millis();

  // Log data periodically based on the defined interval
  if (now - last_log_time >= LOG_INTERVAL_MS) {
    logNow();
    last_log_time = now;
  }

  // Handle serial commands
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'R') {
      retrieveAll();
    }
    if (c == 'L') {
      logNow();
    }
    //reset current index to 0
    if (c == 'C') {
      int current_index = 0;
      EEPROM.put(RUNTIME_VARS_START, current_index);
      Serial.println("Current index reset to 0.");
    }

  }

  delay(1000); // Reduce CPU usage
}

// --- Logging Now Fn --- //
void logNow() {
/*this function */

  int current_index;
  EEPROM.get(RUNTIME_VARS_START, current_index);

  // Stop logging if EEPROM is full
  if (current_index >= MAX_READINGS) {
    Serial.println("EEPROM is full. No more data will be logged.");
    return;
  }

  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  int temp = (int)(tempC * 100); // Multiply by 100 to keep 2 decimal places
  int turb = analogRead(TURBIDITY_PIN);
  turb = convertTurbidity(turb); // Convert turbidity reading to 0-100 scale
  unsigned long timestamp = millis();

  // Calculate EEPROM address for the current reading
  int addr = current_index * 8;

  // Write data to EEPROM
  EEPROM.put(addr, timestamp);
  EEPROM.put(addr + 4, temp);
  EEPROM.put(addr + 6, turb);

  // Update current_index
  current_index++;
  EEPROM.put(RUNTIME_VARS_START, current_index);

  // Print the logged data and timestamp to Serial Monitor
  Serial.print("Logged data: ");
  Serial.print("Index=");
  Serial.print(current_index - 1);
  Serial.print(" Logged - Temp=");
  Serial.print(temp / 100.0);
  Serial.print(", Turbidity=");
  Serial.print(turb);
  Serial.print(", Timestamp=");
  String timestamp_string = turnTimestampToString(timestamp);
  Serial.println(timestamp_string);
}

// --- Retrieval Fn --- //
void retrieveAll() {
  int current_index;
  EEPROM.get(RUNTIME_VARS_START, current_index);

  for (int i = 0; i < current_index; i++) {
    int addr = i * 8;
    unsigned long timestamp;
    int temp, turb;

    EEPROM.get(addr, timestamp);
    EEPROM.get(addr + 4, temp);
    EEPROM.get(addr + 6, turb);

    Serial.print("Reading ");
    Serial.print(i);
    Serial.print(": Temp=");
    Serial.print(temp / 100.0);
    Serial.print(", Turbidity=");
    Serial.print(turb);
    Serial.print(", Timestamp=");
    Serial.println(timestamp);
  }
}

//function that turns turbidity reading from 740-0 to 100-0. Capped at 100 and 0
int convertTurbidity(int turbidity) {
  // Map the turbidity value from 0-740 to 0-100
  int converted = map(turbidity, 0, 740, 0, 100);
  
  // Cap the value at 100 and 0
  if (converted > 100) {
    converted = 100;
  } else if (converted < 0) {
    converted = 0;
  }
  
  return converted;
}

String turnTimestampToString(unsigned long timestamp) {
  // Convert milliseconds to seconds
  unsigned long seconds = timestamp / 1000;
  // Convert seconds to hours, minutes, and seconds
  unsigned long hours = seconds / 3600;
  unsigned long minutes = (seconds % 3600) / 60;
  unsigned long secs = seconds % 60;
  
  // Format the string
  char buffer[50];
  snprintf(buffer, sizeof(buffer), "%02lu:%02lu:%02lu", hours, minutes, secs);
  
  return String(buffer);
}
