from flask import Flask, jsonify, render_template, send_file
import serial
import time
import logging
import os
import sys
import json

# Set up logging configuration to log to console
logging.basicConfig(level=logging.DEBUG, format="%(asctime)s - %(levelname)s - %(message)s")

app = Flask(__name__)

class ArduinoDataRetriever:
    def __init__(self, port='COM3', baudrate=9600, timeout=10):
        try:
            logging.info(f"Initializing connection to Arduino on port {port}...")
            self.ser = serial.Serial(port, baudrate, timeout=timeout)
            self.ser.reset_input_buffer()  # Clears cached data in the serial buffer
            time.sleep(5)  # Wait for Arduino to reset
            logging.info("Successfully connected to Arduino.")
        except serial.SerialException as e:
            logging.error(f"Failed to connect to Arduino: {e}")
            raise

    def test_connection(self):
        """
        Test the connection to the Arduino (for debugging purposes).
        """
        try:
            self.ser.reset_input_buffer()
            logging.info("Testing connection to Arduino...")
            self.ser.write(b'T')  # Example command for testing
            time.sleep(0.5)
            if self.ser.in_waiting > 0:
                response = self.ser.readline().decode('utf-8').strip()
                logging.info(f"Arduino response: {response}")
            else:
                logging.warning("No response from Arduino.")
        except Exception as e:
            logging.error(f"Error during connection test: {e}")
            raise

    def retrieve_data(self):
        """
        Retrieve all stored data from the Arduino using the 'D' command.
        The data is expected to be in CSV format: timestamp,temperature,turbidity.
        """
        try:
            self.ser.reset_input_buffer()
            self.ser.write(b'D')  # Command to retrieve all stored data
            time.sleep(0.5)  # Small delay to let Arduino respond

            logging.info("Retrieving stored data from Arduino...")
            data = []
            while True:
                if self.ser.in_waiting > 0:
                    line = self.ser.readline().decode('utf-8').strip()
                    logging.debug(f"Received line: {line}")
                    if line == "END":
                        logging.info("End of data received.")
                        break
                    else:
                        # Parse the data (assuming it's in CSV format: timestamp,temperature,turbidity)
                        try:
                            timestamp, temperature, turbidity = map(float, line.split(","))
                            data.append({
                                "timestamp": int(timestamp),
                                "temperature": temperature,
                                "turbidity": turbidity
                            })
                        except ValueError:
                            logging.error(f"Invalid data format: {line}")
            return data
        except Exception as e:
            logging.error(f"Error retrieving data: {e}")
            raise

    def read_now(self):
        """
        Read the current sensor data from the Arduino.
        """
        try:
            self.ser.reset_input_buffer()
            self.ser.write(b'L')  # Command to log now and respond
            logging.info("Reading current sensor data from Arduino...")

            today_data = []
            while True:
                if self.ser.in_waiting > 0:
                    line = self.ser.readline().decode('utf-8').strip()
                    logging.debug(f"Received line: {line}")
                    if line == "END":
                        break
                    elif line:
                        today_data.append(line)
            return "\n".join(today_data)
        except Exception as e:
            logging.error(f"Error reading current data: {e}")
            raise


# Instantiate the ArduinoDataRetriever at the start of the program
try:
    arduino_retriever = ArduinoDataRetriever()
except Exception as e:
    logging.critical("Failed to initialize ArduinoDataRetriever. Exiting.")
    sys.exit(1)


@app.route("/")
def index():
    logging.info("Rendering index page...")
    return render_template("index.html")


@app.route("/data")
def data():
    logging.info("Accessing /data endpoint...")
    try:
        stored_data = arduino_retriever.retrieve_data()
        logging.info(f"Retrieved data: {stored_data}")
        return jsonify(stored_data)
    except Exception as e:
        logging.error(f"Error retrieving data: {e}")
        return jsonify({"error": "Failed to retrieve data from Arduino."})


@app.route("/download")
def download():
    logging.info("Accessing /download endpoint...")
    try:
        stored_data = arduino_retriever.retrieve_data()

        # Save the data to a JSON file
        script_dir = os.path.dirname(os.path.abspath(__file__))
        json_file_path = os.path.join(script_dir, "data.json")
        with open(json_file_path, "w") as f:
            json.dump(stored_data, f)

        return send_file(json_file_path, as_attachment=True)
    except Exception as e:
        logging.error(f"Error generating JSON file: {e}")
        return jsonify({"error": "Failed to generate JSON file."})


if __name__ == "__main__":
    app.run(debug=False)