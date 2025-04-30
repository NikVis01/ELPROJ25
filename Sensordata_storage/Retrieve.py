### --- Python Script for retrieving stored sensor data from arduino nano --- ###

import serial

ser = serial.Serial('COM3', 9600)  # Replace with your Arduino port, 9600 refers to baud rate
ser.write(b'R')  # Asking arduino to send stored data

while True:
    line = ser.readline().decode().strip()
    if line:
        print(line)
    else:
        break
