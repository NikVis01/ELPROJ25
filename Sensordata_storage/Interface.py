### --- Python Script for retrieving stored sensor data from arduino nano --- ###
### OBS: Don't have the arduino serial monitor open when running this script! ###

import serial
import time

class ArduinoDataRetriever:
    def __init__(self, port='/dev/ttyUSB0', baudrate=9600, timeout=10):
        self.ser = serial.Serial(port, baudrate)
        self.ser.reset_input_buffer() # Clears cached data in the serial buffer and old data
        time.sleep(5)  # Wait for Arduino to reset

    def test_connection(self):
        # Mostly just for debugging 
        pass

    def retrieve_data(self):
        self.ser.reset_input_buffer()
        self.ser.write(b'R')  # Command to retrieve all stored data
        time.sleep(0.5)  # Small delay to let Arduino respond

        print("Retrieving stored data:\n")
        while True:
            if self.ser.in_waiting > 0:
                line = self.ser.readline().decode('utf-8').strip()
                if line == "END":
                    print("--- End of data ---")
                    break
                else:
                    print(line)


    def read_now(self):
        self.ser.reset_input_buffer()  # Clear old junk
        self.ser.write(b'L')  # Command to log now and respond

        today_data = ""
        while True:
            if self.ser.in_waiting > 0:
                line = self.ser.readline().decode('utf-8').strip()
                if line == "END":
                    break
                elif line:
                    print(line)
                    today_data += line + "\n"
        return today_data

if __name__ == "__main__":
    print("Interface for BojBott\n")

    retriever = ArduinoDataRetriever()

    ### --- For testing read now and store purposes:
    retriever.read_now()

    ### --- For testing connection and debugging:
    # retriever.test_connection()

    ### --- For reading collected data:
    # arduino.retrieve_data()