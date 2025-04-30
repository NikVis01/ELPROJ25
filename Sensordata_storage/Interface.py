### --- Python Script for retrieving stored sensor data from arduino nano --- ###

import serial

class ArduinoDataRetriever:
    def __init__(self, port='/dev/ttyUSB0', baudrate=9600, timeout=10):
        self.ser = serial.Serial(port, baudrate)
        self.ser.reset_input_buffer() # Clears cached data in the serial buffer and old data

    def test_connection(self):
        # Mostly just for debugging 
        pass

    def retrieve_data(self):
        self.ser.write(b'R')  # Command to retrieve data
        while True:
            if self.ser.in_waiting > 0:
                line = self.ser.readline().decode('utf-8').rstrip()
                if line == "END":
                    print("Reading complete")
                    break
                elif line == "END": # Accounting for this in the arduino code, so it'll print "END" when all data is read by retrieveAll function
                    print("Reading complete")
                    break

                else:
                    print("No data received")
                    break

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

    retriever = ArduinoDataRetriever(port='/dev/ttyUSB0', baudrate=9600)

    ### --- For testing read now and store purposes:
    retriever.read_now()

    ### --- For testing connection and debugging:
    # retriever.test_connection()

    ### --- For reading collected data:
    # arduino.retrieve_data()