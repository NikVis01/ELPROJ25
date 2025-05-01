### PROJECT FOLDER FOR ELECTRO PROJECT 2025 

## Code structure:
# 1. Get_AT_commands / Get_AT_commands
Communicates with the SIM800L back and forth through Tx and Rx and vice versa on Nano and SIM800L respectively
Used for debugging of network connectivity and checking signal strength
This code never worked because of what is assumed to be a faulty GSM module

# 2. SMS_send_test
Function is apparent

# 3. Sensordata_storage
Arduino IDE script for reading sensor data, logging them in non-volatile memory on the nano and allowing it to be read

## Documentation for components:

# DS18B20 Thermometer: 
https://www.analog.com/media/en/technical-documentation/data-sheets/ds18b20.pdf

# DFRobot Gravity Turbidity sensor:
https://wiki.dfrobot.com/Turbidity_sensor_SKU__SEN0189

