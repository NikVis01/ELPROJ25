// --- Sketch for continous sensor data logging and storage on-board the Arduino Nano in EEPROM (non-voletile memory)
// 1. Reads Turbidity and Temperature sensor outputs once a day
// 2. Stores the readings in memory
// 3. Sets limit for max days (1k bytes should be enough for 100+ days of sensor data)
// 4. Lets user retrieve sensor readings later by plugging in a laptop and running Retrieve.py (found on github)


//// Hardware Specs: ////

// Turbidity sensor: DFRobot Gravity turbidity sensor (analog pin 6 on nano)
// Thermometer: Elektrokit DS18B20 thermometer (digital pin 11 on nano)
 
// --- Considerations: EEPROM cell will be worn down with time if written to every day, but once a day is well within safe margins (EEPROM is rated for 100k write-cycles per cell)

#include <EEPROM.h>

#include <OneWire.h>
#include <DallasTemperature.h>

//// --- Defnining global variables & Pins --- // 
const int TEMP_PIN = 11; // Digital Pin 11
const int TURBIDITY_PIN = A6; // Double check which pins i put these in
const byte DAYS_TO_STORE = 7; // Starting with 7 for now
const unsigned long ONE_DAY_MS = 24UL * 60UL * 60UL * 1000UL; // 24 hours, unsigned

// --- Init temp sensor --- //
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//// --- Memory allocation & Runtime state variables --- ////
// addresses 0-31 are allocated for sensor readings (turbidity and temperature)
// Assuming sensors require 2bytes each we have (2+2)*7 = 28 (starting at 0 gives address 27 as final address for sensors) and adding a buffer at addresses 28-32 means we can store runtime vars in 32+
 
int current_day = 0;
unsigned long last_log_time = 0;

void setup() {
  Serial.begin(9600);
  sensors.begin(); // Calling init temp sensor type shit
  
  while (!Serial);  // Wait for Serial if connected
  EEPROM.get(32, current_day); // Writing current_day and last_log_time to addresses far enough away for those reserved for sensor data to account for sensor data storage 
  EEPROM.get(34, last_log_time); // Accounting for rough size estimate of runtime vars (2 bytes)

}

// --- Main Loop --- //
void loop() {
  // Designed to continously check (once per sec) if its time to read the data from the sensors, depending on how much time has passed since latest log time
  unsigned long now = millis(); // millis() tracks no. of seconds since system init (nano powered on)
  if ((now - last_log_time >= ONE_DAY_MS) && current_day < DAYS_TO_STORE) {
    logToday();
    last_log_time = now;
    EEPROM.put(30, current_day);
    EEPROM.put(32, last_log_time);
  }
  if (Serial.available()) {
  char c = Serial.read();
    if (c == 'R') {
      retrieveAll();
    }
    if (c == 'L') {
      logNow();
    }
  }

  delay(1000); // Currently checking once per second for testing, but this can be increased to minimize idle CPU usage

}

// --- Logging Today Fn --- //
void logToday() {
  // Reads sensor data and puts them in allocated memory
  // Each read returns a 10 bit val, corresponding to 2 bytes
  int temp = digitalRead(TEMP_PIN);
  int turb = analogRead(TURBIDITY_PIN);
  int addr = current_day * 4; // Calculating address offset, each day takes 4 bytes so day 0 -> addr 0-3, day 1 -> addr 4-7, etc...
  EEPROM.put(addr, temp);
  EEPROM.put(addr + 2, turb);
  current_day++; // Incrementing day
}

// --- Logging Now Fn --- //
void logNow() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  int temp = (int)(tempC * 100);
  int turb = analogRead(TURBIDITY_PIN);

  EEPROM.put(0, temp);
  EEPROM.put(2, turb);

  Serial.print("Today - Temp=");
  Serial.print(temp / 100.0);
  Serial.print(", Turbidity=");
  Serial.println(turb);
  Serial.println("END");
}

// --- Retrieval Fn --- //
void retrieveAll() {
  // Loops through readings for each "day" and prints them in the serial
  for (int i = 0; i < current_day; i++) {
    int temp, turb;
    EEPROM.get(i * 4, temp);
    EEPROM.get(i * 4 + 2, turb);
    Serial.print("Day ");
    Serial.print(i);
    Serial.print(": Temp=");
    Serial.print(temp / 100.0);
    Serial.print(", Turbidity=");
    Serial.println(turb);
    Serial.println("END"); // Will be picked up by python script to terminate read loop
  }
}
