// --- Sketch for continous sensor data logging and storage on-board the Arduino Nano in EEPROM (non-voletile memory)
// 1. Reads Turbidity and Temperature sensor outputs once a day
// 2. Stores the readings in memory
// 3. Sets limit for max days (1k bytes should be enough for 100+ days of sensor data)
// 4. Lets user retrieve sensor readings later by plugging in a laptop and running Retrieve.py (found on github)

// --- Considerations: EEPROM cell will be worn down with time if written to every day, but once a day is well within safe margins (EEPROM is rated for 100k write-cycles per cell)


//// --- Defnining global variables & Pins --- // 
const int TEMP_PIN = A0;
const int TURBIDITY_PIN = A1; // Double check which pins i put these in
const byte DAYS_TO_STORE = 7; // Starting with 7
const unsigned long ONE_DAY_MS = 24UL * 60UL * 60UL * 1000UL; // 24 hours, unsigned for ovf mitigation

//// --- Memory allocation & Runtime state variables ---- ////
// addresses 0-31 are allocated for sensor readings (turbidity and temperature)
// Assuming sensors require 2bytes each we have (2+2)*7 = 28 (starting at 0 gives address 27 as final address for sensors) and adding a buffer at addresses 28-32 means we can store runtime vars in 32+
 
int current_day = 0;
unsigned long last_log_time = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);  // Wait for Serial if connected
  EEPROM.get(32, current_day); // Writing current_day and last_log_time to addresses far enough away for those reserved for sensor data to account for sensor data storage 
  EEPROM.get(34, last_log_time); // Accounting for rough size estimate of runtime vars (2 bytes)


}

void loop() {
  // put your main code here, to run repeatedly:

}
